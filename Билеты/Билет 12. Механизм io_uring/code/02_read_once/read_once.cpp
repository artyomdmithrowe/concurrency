#include <liburing.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <string_view>

int main() {
  int sockets[2]{};
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
    std::perror("socketpair");
    return 1;
  }

  constexpr std::string_view text = "hello from socket through io_uring\n";
  if (write(sockets[0], text.data(), text.size()) == -1) {
    std::perror("write");
    close(sockets[0]);
    close(sockets[1]);
    return 1;
  }

  io_uring ring{};
  int ret = io_uring_queue_init(8, &ring, 0);
  if (ret < 0) {
    std::cerr << "io_uring_queue_init: " << std::strerror(-ret) << '\n';
    close(sockets[0]);
    close(sockets[1]);
    return 1;
  }

  std::array<char, 128> buffer{};
  io_uring_sqe* sqe = io_uring_get_sqe(&ring);
  io_uring_prep_read(sqe, sockets[1], buffer.data(), buffer.size(), 0);
  sqe->user_data = 1;

  ret = io_uring_submit(&ring);
  if (ret < 0) {
    std::cerr << "io_uring_submit: " << std::strerror(-ret) << '\n';
    io_uring_queue_exit(&ring);
    close(sockets[0]);
    close(sockets[1]);
    return 1;
  }

  io_uring_cqe* cqe = nullptr;
  ret = io_uring_wait_cqe(&ring, &cqe);
  if (ret < 0) {
    std::cerr << "io_uring_wait_cqe: " << std::strerror(-ret) << '\n';
    io_uring_queue_exit(&ring);
    close(sockets[0]);
    close(sockets[1]);
    return 1;
  }

  if (cqe->res > 0) {
    std::cout << "read completed, bytes = " << cqe->res << '\n';
    std::cout.write(buffer.data(), cqe->res);
  } else if (cqe->res == 0) {
    std::cout << "read completed: EOF\n";
  } else {
    std::cout << "read failed: " << std::strerror(-cqe->res) << '\n';
  }

  io_uring_cqe_seen(&ring, cqe);
  io_uring_queue_exit(&ring);
  close(sockets[0]);
  close(sockets[1]);
}
