#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

template <typename T>
class BlockingQueue {
 public:
  void push(T value) {
    {
      std::lock_guard<std::mutex> lock(m_);
      q_.push(std::move(value));
    }

    cv_.notify_one();
  }

  T pop() {
    std::unique_lock<std::mutex> lock(m_);

    cv_.wait(lock, [&] { return !q_.empty(); });

    T value = std::move(q_.front());
    q_.pop();
    return value;
  }

 private:
  std::queue<T> q_;
  std::mutex m_;
  std::condition_variable cv_;
};

int main() {
  BlockingQueue<int> queue;

  std::thread consumer([&] {
    for (int i = 0; i < 5; ++i) {
      std::cout << "pop " << queue.pop() << "\n";
    }
  });

  std::thread producer([&] {
    for (int i = 0; i < 5; ++i) {
      queue.push(i);
      std::cout << "push " << i << "\n";
    }
  });

  producer.join();
  consumer.join();
}
