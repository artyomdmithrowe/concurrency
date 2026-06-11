#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

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

int main() {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    throw std::runtime_error("socket failed");
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(11010);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
    throw std::runtime_error(std::string("bind failed: ") +
                             std::strerror(errno));
  }

  if (listen(server_fd, 128) < 0) {
    throw std::runtime_error("listen failed");
  }

  set_nonblocking(server_fd);

  std::vector<Connection> clients;
  uint64_t loop_ticks = 0;
  uint64_t eagain_reads = 0;

  std::cout << "busy-loop echo server on port 11010\n";

  while (true) {
    ++loop_ticks;

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
      Connection client;
      client.fd = client_fd;
      clients.push_back(std::move(client));
      std::cout << "accepted fd " << client_fd << "\n";
    }

    for (auto& client : clients) {
      char buffer[4096];

      while (true) {
        ssize_t n = read(client.fd, buffer, sizeof(buffer));

        if (n > 0) {
          client.out.append(buffer, buffer + n);
          continue;
        }

        if (n == 0) {
          client.closed = true;
          break;
        }

        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          ++eagain_reads;
          break;
        }

        if (errno == EINTR) {
          continue;
        }

        client.closed = true;
        break;
      }

      while (!client.out.empty()) {
        ssize_t n = write(client.fd, client.out.data(), client.out.size());

        if (n > 0) {
          client.out.erase(0, static_cast<size_t>(n));
          continue;
        }

        if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
          break;
        }

        if (n < 0 && errno == EINTR) {
          continue;
        }

        client.closed = true;
        break;
      }
    }

    clients.erase(std::remove_if(clients.begin(), clients.end(),
                                 [](const Connection& client) {
                                   if (client.closed) {
                                     close(client.fd);
                                     return true;
                                   }
                                   return false;
                                 }),
                  clients.end());

    if (loop_ticks % 1'000'000 == 0) {
      std::cout << "ticks=" << loop_ticks << " clients=" << clients.size()
                << " read EAGAIN=" << eagain_reads << "\n";
    }
  }

  for (const auto& client : clients) {
    close(client.fd);
  }
  close(server_fd);
}
