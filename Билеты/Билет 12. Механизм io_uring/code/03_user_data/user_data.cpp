#include <liburing.h>

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

struct Request {
  int id = 0;
  std::string name;
};

int main() {
  io_uring ring{};
  int ret = io_uring_queue_init(8, &ring, 0);
  if (ret < 0) {
    std::cerr << "io_uring_queue_init: " << std::strerror(-ret) << '\n';
    return 1;
  }

  auto submit_nop = [&](int id, std::string name) {
    auto* request = new Request{id, std::move(name)};

    io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    io_uring_prep_nop(sqe);
    sqe->user_data = reinterpret_cast<std::uint64_t>(request);
  };

  submit_nop(1, "first request");
  submit_nop(2, "second request");

  ret = io_uring_submit(&ring);
  if (ret < 0) {
    std::cerr << "io_uring_submit: " << std::strerror(-ret) << '\n';
    io_uring_queue_exit(&ring);
    return 1;
  }

  for (int i = 0; i < 2; ++i) {
    io_uring_cqe* cqe = nullptr;
    ret = io_uring_wait_cqe(&ring, &cqe);
    if (ret < 0) {
      std::cerr << "io_uring_wait_cqe: " << std::strerror(-ret) << '\n';
      break;
    }

    auto* request = reinterpret_cast<Request*>(cqe->user_data);
    std::cout << "completed id=" << request->id << ", name=" << request->name
              << ", res=" << cqe->res << '\n';

    delete request;
    io_uring_cqe_seen(&ring, cqe);
  }

  io_uring_queue_exit(&ring);
}
