#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <atomic>
#include <iostream>

int futex_wake(int* addr, int count) {
  return syscall(SYS_futex, addr, FUTEX_WAKE_PRIVATE, count, nullptr, nullptr,
                 0);
}

int futex_wait(int* addr, int expected) {
  return syscall(SYS_futex, addr, FUTEX_WAIT_PRIVATE, expected, nullptr,
                 nullptr, 0);
}

class TwoStateMutex {
 public:
  void lock() {
    std::atomic_ref<int> state(state_);
    while (state.exchange(1, std::memory_order_acquire) != 0) {
    }
  }

  void unlock() {
    std::atomic_ref<int> state(state_);
    state.store(0, std::memory_order_release);
    ++wake_calls_;
    futex_wake(&state_, 1);
  }

  int wake_calls() const { return wake_calls_; }

 private:
  int state_ = 0;
  int wake_calls_ = 0;
};

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
      ++wake_calls_;
      futex_wake(&state_, 1);
    }
  }

  int wake_calls() const { return wake_calls_; }

 private:
  int state_ = 0;
  int wake_calls_ = 0;
};

int main() {
  constexpr int iterations = 100;

  TwoStateMutex two_state;
  Mutex3 three_state;

  for (int i = 0; i < iterations; ++i) {
    two_state.lock();
    two_state.unlock();

    three_state.lock();
    three_state.unlock();
  }

  std::cout << "uncontended lock/unlock pairs: " << iterations << "\n";
  std::cout << "two-state futex_wake calls:    " << two_state.wake_calls()
            << "\n";
  std::cout << "three-state futex_wake calls:  " << three_state.wake_calls()
            << "\n";
}
