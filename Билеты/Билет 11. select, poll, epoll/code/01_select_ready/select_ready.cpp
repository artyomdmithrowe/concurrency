#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>

int main() {
  int sockets[2];
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0) {
    throw std::runtime_error("socketpair failed");
  }

  const char message[] = "hello select";
  write(sockets[1], message, sizeof(message) - 1);

  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(sockets[0], &readfds);
  FD_SET(sockets[1], &readfds);

  int max_fd = std::max(sockets[0], sockets[1]);
  int ready = select(max_fd + 1, &readfds, nullptr, nullptr, nullptr);

  std::cout << "select returned: " << ready << "\n";

  for (int fd : sockets) {
    if (FD_ISSET(fd, &readfds)) {
      char buffer[128];
      ssize_t n = read(fd, buffer, sizeof(buffer));
      std::cout << "fd " << fd << " is readable: "
                << std::string(buffer, buffer + n) << "\n";
    } else {
      std::cout << "fd " << fd << " is not readable\n";
    }
  }

  close(sockets[0]);
  close(sockets[1]);
}
