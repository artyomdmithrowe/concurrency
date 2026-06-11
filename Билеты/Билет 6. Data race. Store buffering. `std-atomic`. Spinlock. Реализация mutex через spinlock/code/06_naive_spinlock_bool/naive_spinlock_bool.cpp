#include <iostream>
#include <thread>
#include <vector>

class NaiveSpinLock {
 public:
  void lock() {
    while (state_) {
    }

    state_ = true;
  }

  void unlock() { state_ = false; }

 private:
  bool state_ = false;
};

int main() {
  constexpr int threads_count = 8;
  constexpr int iterations = 200'000;

  NaiveSpinLock lock;
  size_t counter = 0;
  std::vector<std::thread> threads;

  for (int i = 0; i < threads_count; ++i) {
    threads.emplace_back([&] {
      for (int j = 0; j < iterations; ++j) {
        lock.lock();
        ++counter;
        lock.unlock();
      }
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  std::cout << "expected: " << threads_count * iterations << "\n";
  std::cout << "actual:   " << counter << "\n";
}
