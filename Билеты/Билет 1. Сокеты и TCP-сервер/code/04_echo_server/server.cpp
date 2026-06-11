#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
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

void handle_client(int client_fd) {
  char buffer[4096];

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
      std::cout << "client closed connection\n";
      break;
    }

    if (!write_all(client_fd, buffer, static_cast<size_t>(n))) {
      std::cerr << "write failed: " << strerror(errno) << "\n";
      break;
    }
  }

  close(client_fd);
}

int main() {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd < 0) {
    std::cerr << "socket failed: " << strerror(errno) << "\n";
    return 1;
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8081);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
    std::cerr << "bind failed: " << strerror(errno) << "\n";
    close(server_fd);
    return 1;
  }

  if (listen(server_fd, SOMAXCONN) < 0) {
    std::cerr << "listen failed: " << strerror(errno) << "\n";
    close(server_fd);
    return 1;
  }

  std::cout << "Echo server is listening on port 8081\n";

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

    handle_client(client_fd);
  }

  close(server_fd);
}
