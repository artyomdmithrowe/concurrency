#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
  int sockets[2];
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0) {
    throw std::runtime_error("socketpair failed");
  }

  const char message[] = "hello poll";
  write(sockets[1], message, sizeof(message) - 1);

  std::vector<pollfd> fds;
  fds.push_back(pollfd{.fd = sockets[0], .events = POLLIN, .revents = 0});
  fds.push_back(pollfd{.fd = sockets[1], .events = POLLIN, .revents = 0});

  int ready = poll(fds.data(), fds.size(), -1);
  std::cout << "poll returned: " << ready << "\n";

  for (const auto& pfd : fds) {
    if (pfd.revents & POLLIN) {
      char buffer[128];
      ssize_t n = read(pfd.fd, buffer, sizeof(buffer));
      std::cout << "fd " << pfd.fd
                << " has POLLIN: " << std::string(buffer, buffer + n) << "\n";
    } else {
      std::cout << "fd " << pfd.fd << " has no POLLIN\n";
    }
  }

  close(sockets[0]);
  close(sockets[1]);
}
