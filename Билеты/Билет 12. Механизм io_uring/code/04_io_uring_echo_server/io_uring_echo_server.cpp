#include <liburing.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

constexpr int kPort = 12000;
constexpr int kQueueDepth = 256;

enum class OpType {
  Accept,
  Recv,
  Send,
};

struct Connection {
  int fd = -1;
  std::array<char, 4096> buffer{};
  std::string out;
  std::size_t sent = 0;
};

struct Request {
  OpType type;
  Connection* conn = nullptr;
};

void check_submit(int ret) {
  if (ret < 0) {
    throw std::runtime_error(std::string("io_uring_submit: ") +
                             std::strerror(-ret));
  }
}

int make_server_socket() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    throw std::runtime_error(std::string("socket: ") + std::strerror(errno));
  }

  int op = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(kPort);

  if (bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
    close(fd);
    throw std::runtime_error(std::string("bind: ") + std::strerror(errno));
  }

  if (listen(fd, SOMAXCONN) == -1) {
    close(fd);
    throw std::runtime_error(std::string("listen: ") + std::strerror(errno));
  }

  return fd;
}

io_uring_sqe* get_sqe(io_uring& ring) {
  io_uring_sqe* sqe = io_uring_get_sqe(&ring);
  if (sqe == nullptr) {
    throw std::runtime_error("io_uring_get_sqe failed");
  }

  return sqe;
}

void submit_accept(io_uring& ring, int server_fd) {
  auto* req = new Request{OpType::Accept, nullptr};

  io_uring_sqe* sqe = get_sqe(ring);
  io_uring_prep_accept(sqe, server_fd, nullptr, nullptr, 0);
  sqe->user_data = reinterpret_cast<std::uint64_t>(req);

  check_submit(io_uring_submit(&ring));
}

void submit_recv(io_uring& ring, Connection* conn) {
  auto* req = new Request{OpType::Recv, conn};

  io_uring_sqe* sqe = get_sqe(ring);
  io_uring_prep_recv(sqe, conn->fd, conn->buffer.data(), conn->buffer.size(),
                     0);
  sqe->user_data = reinterpret_cast<std::uint64_t>(req);

  check_submit(io_uring_submit(&ring));
}

void submit_send(io_uring& ring, Connection* conn) {
  auto* req = new Request{OpType::Send, conn};

  io_uring_sqe* sqe = get_sqe(ring);
  const char* data = conn->out.data() + conn->sent;
  std::size_t size = conn->out.size() - conn->sent;
  io_uring_prep_send(sqe, conn->fd, data, size, MSG_NOSIGNAL);
  sqe->user_data = reinterpret_cast<std::uint64_t>(req);

  check_submit(io_uring_submit(&ring));
}

void close_connection(Connection* conn) {
  if (conn == nullptr) {
    return;
  }

  if (conn->fd != -1) {
    close(conn->fd);
  }

  delete conn;
}

void handle_accept(io_uring& ring, int server_fd, int res) {
  submit_accept(ring, server_fd);

  if (res < 0) {
    std::cerr << "accept failed: " << std::strerror(-res) << '\n';
    return;
  }

  auto* conn = new Connection;
  conn->fd = res;
  std::cout << "client connected, fd=" << conn->fd << '\n';
  submit_recv(ring, conn);
}

void handle_recv(io_uring& ring, Connection* conn, int res) {
  if (res <= 0) {
    if (res < 0) {
      std::cerr << "recv failed: " << std::strerror(-res) << '\n';
    }

    close_connection(conn);
    return;
  }

  conn->out.assign(conn->buffer.data(), res);
  conn->sent = 0;
  submit_send(ring, conn);
}

void handle_send(io_uring& ring, Connection* conn, int res) {
  if (res <= 0) {
    if (res < 0) {
      std::cerr << "send failed: " << std::strerror(-res) << '\n';
    }
    close_connection(conn);
    return;
  }

  conn->sent += static_cast<std::size_t>(res);
  if (conn->sent < conn->out.size()) {
    submit_send(ring, conn);
    return;
  }

  conn->out.clear();
  conn->sent = 0;
  submit_recv(ring, conn);
}

}  // namespace

int main() {
  try {
    int server_fd = make_server_socket();

    io_uring ring{};
    int ret = io_uring_queue_init(kQueueDepth, &ring, 0);
    if (ret < 0) {
      close(server_fd);
      std::cerr << "io_uring_queue_init: " << std::strerror(-ret) << '\n';
      return 1;
    }

    submit_accept(ring, server_fd);
    std::cout << "io_uring echo server on port " << kPort << '\n';

    while (true) {
      io_uring_cqe* cqe = nullptr;
      ret = io_uring_wait_cqe(&ring, &cqe);
      if (ret < 0) {
        std::cerr << "io_uring_wait_cqe: " << std::strerror(-ret) << '\n';
        continue;
      }

      auto* req = reinterpret_cast<Request*>(cqe->user_data);
      int res = cqe->res;
      io_uring_cqe_seen(&ring, cqe);

      if (req->type == OpType::Accept) {
        handle_accept(ring, server_fd, res);
      } else if (req->type == OpType::Recv) {
        handle_recv(ring, req->conn, res);
      } else if (req->type == OpType::Send) {
        handle_send(ring, req->conn, res);
      }

      delete req;
    }

    io_uring_queue_exit(&ring);
    close(server_fd);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
