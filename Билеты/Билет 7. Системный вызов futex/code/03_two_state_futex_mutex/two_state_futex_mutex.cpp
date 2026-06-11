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

class FutexMutex {
 public:
  void lock() {
    std::atomic_ref<int> state(state_);

    if (state.exchange(1, std::memory_order_acquire) == 0) {
      return;
    }

    while (state.exchange(1, std::memory_order_acquire) != 0) {
      futex_wait(&state_, 1);
    }
  }

  void unlock() {
    std::atomic_ref<int> state(state_);
    state.store(0, std::memory_order_release);
    futex_wake(&state_, 1);
  }

 private:
  int state_ = 0;
};

int main() {
  constexpr int threads_count = 8;
  constexpr int iterations = 200'000;

  FutexMutex mutex;
  size_t counter = 0;
  std::vector<std::thread> threads;

  for (int i = 0; i < threads_count; ++i) {
    threads.emplace_back([&] {
      for (int j = 0; j < iterations; ++j) {
        std::lock_guard<FutexMutex> guard(mutex);
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
