#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

#if defined(__x86_64__) || defined(__i386__)
#include <immintrin.h>
#endif

void spin_pause() {
#if defined(__x86_64__) || defined(__i386__)
  _mm_pause();
#else
  std::this_thread::yield();
#endif
}

class TtasSpinLock {
 public:
  void lock() {
    while (true) {
      while (state_.load(std::memory_order_relaxed)) {
        spin_pause();
      }

      if (!state_.exchange(true, std::memory_order_acquire)) {
        return;
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

  TtasSpinLock lock;
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
