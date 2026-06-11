#include <liburing.h>

#include <cerrno>
#include <cstring>
#include <iostream>

int main() {
  io_uring ring{};

  int ret = io_uring_queue_init(8, &ring, 0);
  if (ret < 0) {
    std::cerr << "io_uring_queue_init: " << std::strerror(-ret) << '\n';
    return 1;
  }

  io_uring_sqe* sqe = io_uring_get_sqe(&ring);
  if (sqe == nullptr) {
    std::cerr << "io_uring_get_sqe failed\n";
    io_uring_queue_exit(&ring);
    return 1;
  }

  io_uring_prep_nop(sqe);
  sqe->user_data = 42;

  ret = io_uring_submit(&ring);
  if (ret < 0) {
    std::cerr << "io_uring_submit: " << std::strerror(-ret) << '\n';
    io_uring_queue_exit(&ring);
    return 1;
  }

  io_uring_cqe* cqe = nullptr;
  ret = io_uring_wait_cqe(&ring, &cqe);
  if (ret < 0) {
    std::cerr << "io_uring_wait_cqe: " << std::strerror(-ret) << '\n';
    io_uring_queue_exit(&ring);
    return 1;
  }

  std::cout << "completion user_data = " << cqe->user_data << '\n';
  std::cout << "completion res       = " << cqe->res << '\n';

  io_uring_cqe_seen(&ring, cqe);
  io_uring_queue_exit(&ring);
}
