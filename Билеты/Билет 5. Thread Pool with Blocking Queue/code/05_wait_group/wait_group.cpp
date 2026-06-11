#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class WaitGroup {
 public:
  void add(size_t n) {
    std::lock_guard<std::mutex> lock(m_);
    count_ += n;
  }

  void done() {
    {
      std::lock_guard<std::mutex> lock(m_);
      --count_;
    }
    cv_.notify_all();
  }

  void wait() {
    std::unique_lock<std::mutex> lock(m_);
    cv_.wait(lock, [this] { return count_ == 0; });
  }

 private:
  size_t count_ = 0;
  std::mutex m_;
  std::condition_variable cv_;
};

int main() {
  WaitGroup wg;
  std::vector<std::thread> threads;

  wg.add(5);

  for (int i = 0; i < 5; ++i) {
    threads.emplace_back([&, i] {
      std::cout << "work #" << i << "\n";
      wg.done();
    });
  }

  wg.wait();

  for (auto& thread : threads) {
    thread.join();
  }

  std::cout << "all tasks done\n";
}
