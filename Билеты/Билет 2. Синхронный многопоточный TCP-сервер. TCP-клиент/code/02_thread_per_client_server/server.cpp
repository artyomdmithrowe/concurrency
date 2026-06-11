#include <arpa/inet.h>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

bool write_all(int fd, const char *data, size_t size) {
  size_t written = 0;

  while (written < size) {
    ssize_t res = write(fd, data + written, size - written);

    if (res < 0) {
      if (errno == EINTR) {
        continue;
      }

      return false;
    }

    if (res == 0) {
      return false;
    }

    written += static_cast<size_t>(res);
  }

  return true;
}

void handle_client(int client_fd, int client_id) {
  char buffer[4096];

  std::cout << "client #" << client_id << " handler started\n";

  while (true) {
    ssize_t n = read(client_fd, buffer, sizeof(buffer));

    if (n < 0) {
      if (errno == EINTR) {
        continue;
      }

      std::cerr << "read failed: " << strerror(errno) << "\n";
      break;
    }

    if (n == 0) {
      std::cout << "client #" << client_id << " disconnected\n";
      break;
    }

    if (!write_all(client_fd, buffer, static_cast<size_t>(n))) {
      std::cerr << "write failed\n";
      break;
    }
  }

  close(client_fd);
}

int main() {
  signal(SIGPIPE, SIG_IGN);

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd < 0) {
    std::cerr << "socket failed: " << strerror(errno) << "\n";
    return 1;
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(11000);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
    std::cerr << "bind failed: " << strerror(errno) << "\n";
    close(server_fd);
    return 1;
  }

  if (listen(server_fd, 10) < 0) {
    std::cerr << "listen failed: " << strerror(errno) << "\n";
    close(server_fd);
    return 1;
  }

  std::cout << "server is listening on port 11000\n";

  int client_counter = 0;

  while (true) {
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(
        server_fd, reinterpret_cast<sockaddr *>(&client_addr), &client_len);

    if (client_fd < 0) {
      if (errno == EINTR) {
        continue;
      }

      std::cerr << "accept failed: " << strerror(errno) << "\n";
      continue;
    }

    char ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));

    int client_id = client_counter++;

    std::cout << "new client #" << client_id << " from " << ip << ":"
              << ntohs(client_addr.sin_port) << "\n";

    std::thread t(handle_client, client_fd, client_id);
    t.detach();
  }

  close(server_fd);
}
