#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <atomic>
#include <iostream>

int futex_wake(int* addr, int count) {
  return syscall(SYS_futex, addr, FUTEX_WAKE_PRIVATE, count, nullptr, nullptr,
                 0);
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
    woken_total_ += futex_wake(&state_, 1);
  }

  int wake_calls() const { return wake_calls_; }
  int woken_total() const { return woken_total_; }

 private:
  int state_ = 0;
  int wake_calls_ = 0;
  int woken_total_ = 0;
};

int main() {
  constexpr int iterations = 10;

  TwoStateMutex mutex;

  for (int i = 0; i < iterations; ++i) {
    mutex.lock();
    mutex.unlock();
  }

  std::cout << "lock/unlock pairs: " << iterations << "\n";
  std::cout << "futex_wake calls:  " << mutex.wake_calls() << "\n";
  std::cout << "actually woken:    " << mutex.woken_total() << "\n";
}
