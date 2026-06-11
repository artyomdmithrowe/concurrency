#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

template <typename T>
class BlockingQueue {
 public:
  void push(T value) {
    {
      std::lock_guard<std::mutex> lock(m_);
      if (closed_) {
        return;
      }
      q_.push(std::move(value));
    }

    cv_.notify_one();
  }

  bool pop(T& out) {
    std::unique_lock<std::mutex> lock(m_);

    cv_.wait(lock, [&] { return closed_ || !q_.empty(); });

    if (q_.empty()) {
      return false;
    }

    out = std::move(q_.front());
    q_.pop();
    return true;
  }

  void close() {
    {
      std::lock_guard<std::mutex> lock(m_);
      closed_ = true;
    }

    cv_.notify_all();
  }

 private:
  std::queue<T> q_;
  bool closed_ = false;
  std::mutex m_;
  std::condition_variable cv_;
};

int main() {
  BlockingQueue<int> queue;
  std::vector<std::thread> workers;

  for (int i = 0; i < 3; ++i) {
    workers.emplace_back([&, i] {
      int value = 0;
      while (queue.pop(value)) {
        std::cout << "worker #" << i << " got " << value << "\n";
      }
      std::cout << "worker #" << i << " exits\n";
    });
  }

  for (int value = 0; value < 6; ++value) {
    queue.push(value);
  }

  queue.close();

  for (auto& worker : workers) {
    worker.join();
  }
}
