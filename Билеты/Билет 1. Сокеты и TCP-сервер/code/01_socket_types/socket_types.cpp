#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

void print_result(const char *name, int fd) {
  if (fd < 0) {
    std::cerr << name << " failed: " << strerror(errno) << "\n";
    return;
  }

  std::cout << name << " fd = " << fd << "\n";
  close(fd);
}

int main() {
  int tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
  print_result("TCP socket(AF_INET, SOCK_STREAM, 0)", tcp_fd);

  int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
  print_result("UDP socket(AF_INET, SOCK_DGRAM, 0)", udp_fd);

  int unix_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  print_result("Unix socket(AF_UNIX, SOCK_STREAM, 0)", unix_fd);
}
