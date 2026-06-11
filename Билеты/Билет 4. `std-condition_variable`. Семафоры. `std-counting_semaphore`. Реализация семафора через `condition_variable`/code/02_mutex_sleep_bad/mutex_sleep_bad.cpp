#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class SleepSemaphore {
public:
  explicit SleepSemaphore(int initial) : count_(initial) {}

  void acquire() {
    std::unique_lock<std::mutex> lock(m_);

    while (count_ == 0) {
      lock.unlock();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      lock.lock();
    }

    --count_;
  }

  void release() {
    std::lock_guard<std::mutex> lock(m_);
    ++count_;
  }

private:
  int count_;
  std::mutex m_;
};

int main() {
  SleepSemaphore sem(2);
  std::vector<std::thread> threads;

  for (int i = 0; i < 6; ++i) {
    threads.emplace_back([&, i] {
      sem.acquire();
      std::cout << "entered #" << i << "\n";
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      sem.release();
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}
