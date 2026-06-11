#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

void set_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);

  if (flags < 0) {
    throw std::runtime_error("fcntl F_GETFL failed");
  }

  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
    throw std::runtime_error("fcntl F_SETFL failed");
  }
}

struct Connection {
  int fd = -1;
  std::string out;
  bool closed = false;
};

void update_events(int epfd, const Connection& connection) {
  epoll_event event{};
  event.events = EPOLLIN | EPOLLRDHUP;

  if (!connection.out.empty()) {
    event.events |= EPOLLOUT;
  }

  event.data.fd = connection.fd;

  if (epoll_ctl(epfd, EPOLL_CTL_MOD, connection.fd, &event) < 0) {
    throw std::runtime_error("epoll_ctl MOD failed");
  }
}

void close_connection(int epfd, std::unordered_map<int, Connection>& conns,
                      int fd) {
  epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
  close(fd);
  conns.erase(fd);
  std::cout << "closed fd " << fd << "\n";
}

void handle_read(Connection& connection) {
  char buffer[4096];

  while (true) {
    ssize_t n = read(connection.fd, buffer, sizeof(buffer));

    if (n > 0) {
      connection.out.append(buffer, buffer + n);
      continue;
    }

    if (n == 0) {
      connection.closed = true;
      return;
    }

    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return;
    }

    if (errno == EINTR) {
      continue;
    }

    connection.closed = true;
    return;
  }
}

void handle_write(Connection& connection) {
  while (!connection.out.empty()) {
    ssize_t n =
        write(connection.fd, connection.out.data(), connection.out.size());

    if (n > 0) {
      connection.out.erase(0, static_cast<size_t>(n));
      continue;
    }

    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      return;
    }

    if (n < 0 && errno == EINTR) {
      continue;
    }

    connection.closed = true;
    return;
  }
}

int main() {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd < 0) {
    throw std::runtime_error("socket failed");
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(11011);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
    throw std::runtime_error(std::string("bind failed: ") +
                             std::strerror(errno));
  }

  if (listen(server_fd, SOMAXCONN) < 0) {
    throw std::runtime_error("listen failed");
  }

  set_nonblocking(server_fd);

  int epfd = epoll_create1(EPOLL_CLOEXEC);
  if (epfd < 0) {
    throw std::runtime_error("epoll_create1 failed");
  }

  epoll_event server_event{};
  server_event.events = EPOLLIN;
  server_event.data.fd = server_fd;

  if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &server_event) < 0) {
    throw std::runtime_error("epoll_ctl ADD server failed");
  }

  std::unordered_map<int, Connection> conns;
  epoll_event events[64];

  std::cout << "epoll echo server on port 11011\n";

  while (true) {
    int ready = epoll_wait(epfd, events, 64, -1);

    if (ready < 0) {
      if (errno == EINTR) {
        continue;
      }

      throw std::runtime_error("epoll_wait failed");
    }

    for (int i = 0; i < ready; ++i) {
      int fd = events[i].data.fd;
      uint32_t mask = events[i].events;

      if (fd == server_fd) {
        while (true) {
          int client_fd = accept(server_fd, nullptr, nullptr);

          if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
              break;
            }

            if (errno == EINTR) {
              continue;
            }

            std::cerr << "accept failed: " << std::strerror(errno) << "\n";
            break;
          }

          set_nonblocking(client_fd);

          epoll_event client_event{};
          client_event.events = EPOLLIN | EPOLLRDHUP;
          client_event.data.fd = client_fd;

          if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &client_event) < 0) {
            close(client_fd);
            continue;
          }

          Connection connection;
          connection.fd = client_fd;
          conns.emplace(client_fd, std::move(connection));
          std::cout << "accepted fd " << client_fd << "\n";
        }

        continue;
      }

      auto it = conns.find(fd);
      if (it == conns.end()) {
        continue;
      }

      Connection& connection = it->second;

      if (mask & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
        connection.closed = true;
      }

      if (!connection.closed && (mask & EPOLLIN)) {
        handle_read(connection);
      }

      if (!connection.closed && (mask & EPOLLOUT)) {
        handle_write(connection);
      }

      if (connection.closed) {
        close_connection(epfd, conns, fd);
      } else {
        update_events(epfd, connection);
      }
    }
  }
}
