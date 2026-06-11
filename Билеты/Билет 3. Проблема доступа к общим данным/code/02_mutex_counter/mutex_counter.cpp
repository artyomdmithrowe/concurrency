#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class Counter {
public:
  void increment() {
    std::lock_guard<std::mutex> lock(mutex_);
    ++value_;
  }

  int get() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return value_;
  }

private:
  mutable std::mutex mutex_;
  int value_ = 0;
};

int main() {
  constexpr int threads_count = 8;
  constexpr int iterations = 500'000;

  Counter counter;
  std::vector<std::thread> threads;

  for (int i = 0; i < threads_count; ++i) {
    threads.emplace_back([&] {
      for (int j = 0; j < iterations; ++j) {
        counter.increment();
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  std::cout << "expected: " << threads_count * iterations << "\n";
  std::cout << "actual:   " << counter.get() << "\n";
}
