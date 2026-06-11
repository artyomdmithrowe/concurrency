#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

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

  std::cout << "Server is listening on port 8081\n";

  while (true) {
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(
        server_fd, reinterpret_cast<sockaddr *>(&client_addr), &client_len);

    if (client_fd < 0) {
      std::cerr << "accept failed: " << strerror(errno) << "\n";
      continue;
    }

    char client_ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));

    std::cout << "Accepted connection from " << client_ip << ":"
              << ntohs(client_addr.sin_port) << "\n";

    char buffer[1024];

    ssize_t n = read(client_fd, buffer, sizeof(buffer) - 1);

    if (n > 0) {
      buffer[n] = '\0';

      std::cout << "Received: " << buffer << "\n";

      const char *response = "Hello from server\n";
      write(client_fd, response, strlen(response));
    }

    close(client_fd);
  }

  close(server_fd);
  return 0;
}
