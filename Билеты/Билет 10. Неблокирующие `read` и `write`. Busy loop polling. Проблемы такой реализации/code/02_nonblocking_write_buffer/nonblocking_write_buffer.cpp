#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

void set_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);

  if (flags < 0) {
    throw std::runtime_error("fcntl F_GETFL failed");
  }

  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
    throw std::runtime_error("fcntl F_SETFL failed");
  }
}

int main() {
  int sockets[2];
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0) {
    throw std::runtime_error("socketpair failed");
  }

  int writer = sockets[0];
  int reader = sockets[1];
  set_nonblocking(writer);

  std::string output_buffer(4 * 1024 * 1024, 'x');
  size_t sent = 0;

  while (!output_buffer.empty()) {
    errno = 0;
    ssize_t n = write(writer, output_buffer.data(), output_buffer.size());

    if (n > 0) {
      sent += static_cast<size_t>(n);
      output_buffer.erase(0, static_cast<size_t>(n));
      continue;
    }

    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      std::cout << "write: EAGAIN/EWOULDBLOCK\n";
      break;
    }

    std::cout << "write: error " << errno << " (" << std::strerror(errno)
              << ")\n";
    break;
  }

  std::cout << "sent bytes:      " << sent << "\n";
  std::cout << "buffer remains:  " << output_buffer.size() << "\n";

  close(writer);
  close(reader);
}
