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

class BadWaitingMutex {
 public:
  void lock() {
    std::atomic_ref<int> state(state_);

    if (state.exchange(1, std::memory_order_acquire) == 0) {
      return;
    }

    waiting_.fetch_add(1, std::memory_order_relaxed);

    while (state.exchange(1, std::memory_order_acquire) == 1) {
      futex_wait(&state_, 1);
    }

    waiting_.fetch_sub(1, std::memory_order_relaxed);
  }

  void unlock() {
    std::atomic_ref<int> state(state_);

    state.store(0, std::memory_order_release);

    if (waiting_.load(std::memory_order_relaxed) > 0) {
      futex_wake(&state_, 1);
    }
  }

 private:
  int state_ = 0;
  std::atomic<int> waiting_{0};
};

int main() {
  constexpr int threads_count = 4;
  constexpr int iterations = 10'000;

  BadWaitingMutex mutex;
  int counter = 0;
  std::vector<std::thread> threads;

  for (int i = 0; i < threads_count; ++i) {
    threads.emplace_back([&] {
      for (int j = 0; j < iterations; ++j) {
        std::lock_guard<BadWaitingMutex> guard(mutex);
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
