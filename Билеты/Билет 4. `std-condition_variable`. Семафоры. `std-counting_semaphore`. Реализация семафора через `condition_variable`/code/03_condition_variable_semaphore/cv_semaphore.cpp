#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class Semaphore {
public:
  explicit Semaphore(int initial) : count_(initial) {}

  void acquire() {
    std::unique_lock<std::mutex> lock(m_);

    cv_.wait(lock, [&] { return count_ > 0; });

    --count_;
  }

  void release() {
    {
      std::lock_guard<std::mutex> lock(m_);
      ++count_;
    }

    cv_.notify_one();
  }

private:
  int count_;
  std::mutex m_;
  std::condition_variable cv_;
};

int main() {
  Semaphore sem(2);
  std::vector<std::thread> threads;

  for (int i = 0; i < 6; ++i) {
    threads.emplace_back([&, i] {
      sem.acquire();
      std::cout << "entered #" << i << "\n";
      std::this_thread::sleep_for(std::chrono::milliseconds(2000));
      std::cout << "leaving #" << i << "\n";
      sem.release();
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}
