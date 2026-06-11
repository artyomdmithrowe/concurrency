#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class SpinMutex {
 public:
  void lock() {
    while (state_.exchange(true, std::memory_order_acquire)) {
      while (state_.load(std::memory_order_relaxed)) {
        std::this_thread::yield();
      }
    }
  }

  void unlock() { state_.store(false, std::memory_order_release); }

 private:
  std::atomic<bool> state_{false};
};

int main() {
  constexpr int threads_count = 8;
  constexpr int iterations = 200'000;

  SpinMutex mutex;
  size_t counter = 0;
  std::vector<std::thread> threads;

  for (int i = 0; i < threads_count; ++i) {
    threads.emplace_back([&] {
      for (int j = 0; j < iterations; ++j) {
        std::lock_guard<SpinMutex> guard(mutex);
        ++counter;
      }
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  std::cout << "expected: " << threads_count * iterations << "\n";
  std::cout << "actual:   " << counter << "\n";
}
