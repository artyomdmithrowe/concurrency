#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>

void set_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);

  if (flags < 0) {
    throw std::runtime_error("fcntl F_GETFL failed");
  }

  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
    throw std::runtime_error("fcntl F_SETFL failed");
  }
}

void print_read_result(int fd) {
  char buffer[128];
  errno = 0;

  ssize_t n = read(fd, buffer, sizeof(buffer));

  if (n > 0) {
    std::cout << "read: " << n << " bytes: " << std::string(buffer, buffer + n)
              << "\n";
  } else if (n == 0) {
    std::cout << "read: EOF\n";
  } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
    std::cout << "read: EAGAIN/EWOULDBLOCK\n";
  } else {
    std::cout << "read: error " << errno << " (" << std::strerror(errno)
              << ")\n";
  }
}

int main() {
  int sockets[2];
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0) {
    throw std::runtime_error("socketpair failed");
  }

  int reader = sockets[0];
  int writer = sockets[1];

  set_nonblocking(reader);

  print_read_result(reader);

  const char message[] = "hello";
  write(writer, message, sizeof(message) - 1);
  print_read_result(reader);

  close(writer);
  print_read_result(reader);

  close(reader);
}
