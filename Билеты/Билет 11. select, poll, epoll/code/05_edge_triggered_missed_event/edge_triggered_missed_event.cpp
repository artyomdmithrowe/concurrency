#include <fcntl.h>
#include <sys/epoll.h>
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

int main() {
  int sockets[2];
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0) {
    throw std::runtime_error("socketpair failed");
  }

  int reader = sockets[0];
  int writer = sockets[1];
  set_nonblocking(reader);

  int epfd = epoll_create1(EPOLL_CLOEXEC);
  if (epfd < 0) {
    throw std::runtime_error("epoll_create1 failed");
  }

  epoll_event event{};
  event.events = EPOLLIN | EPOLLET;
  event.data.fd = reader;
  epoll_ctl(epfd, EPOLL_CTL_ADD, reader, &event);

  const char message[] = "abcdef";
  write(writer, message, sizeof(message) - 1);

  epoll_event events[4];
  int ready = epoll_wait(epfd, events, 4, -1);
  std::cout << "first epoll_wait returned: " << ready << "\n";

  char first_chunk[2];
  ssize_t n = read(reader, first_chunk, sizeof(first_chunk));
  std::cout << "read only: " << std::string(first_chunk, first_chunk + n)
            << "\n";

  ready = epoll_wait(epfd, events, 4, 300);
  std::cout << "second epoll_wait returned: " << ready << "\n";

  while (true) {
    char buffer[16];
    n = read(reader, buffer, sizeof(buffer));

    if (n > 0) {
      std::cout << "manual read got: " << std::string(buffer, buffer + n)
                << "\n";
      continue;
    }

    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      std::cout << "EAGAIN\n";
      break;
    }

    if (n < 0 && errno == EINTR) {
      continue;
    }

    std::cout << "read stopped: " << std::strerror(errno) << "\n";
    break;
  }

  close(epfd);
  close(reader);
  close(writer);
}
