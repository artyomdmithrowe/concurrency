#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

int futex_wait(int* addr, int expected) {
  return syscall(SYS_futex, addr, FUTEX_WAIT_PRIVATE, expected, nullptr,
                 nullptr, 0);
}

int futex_wake(int* addr, int count) {
  return syscall(SYS_futex, addr, FUTEX_WAKE_PRIVATE, count, nullptr, nullptr,
                 0);
}

class Mutex3 {
 public:
  void lock() {
    std::atomic_ref<int> state(state_);
    int current = 0;

    if (state.compare_exchange_strong(current, 1, std::memory_order_acquire)) {
      return;
    }

    while (true) {
      if (current != 2 && state.exchange(2, std::memory_order_acquire) == 0) {
        return;
      }

      futex_wait(&state_, 2);

      current = 0;
      if (state.compare_exchange_strong(current, 2,
                                        std::memory_order_acquire)) {
        return;
      }
    }
  }

  void unlock() {
    std::atomic_ref<int> state(state_);

    if (state.fetch_sub(1, std::memory_order_release) != 1) {
      state.store(0, std::memory_order_release);
      futex_wake(&state_, 1);
    }
  }

 private:
  int state_ = 0;
};

int main() {
  constexpr int threads_count = 8;
  constexpr int iterations = 1'000;

  Mutex3 mutex;
  size_t counter = 0;
  std::vector<std::thread> threads;

  for (int i = 0; i < threads_count; ++i) {
    threads.emplace_back([&] {
      for (int j = 0; j < iterations; ++j) {
        std::lock_guard<Mutex3> guard(mutex);
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
