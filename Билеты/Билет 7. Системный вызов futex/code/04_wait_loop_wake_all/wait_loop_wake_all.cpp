#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

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

    while (state.exchange(1, std::memory_order_acquire) != 0) {
      ++wait_count_;
      futex_wait(&state_, 1);
    }
  }

  void unlock_all() {
    std::atomic_ref<int> state(state_);
    state.store(0, std::memory_order_release);
    futex_wake(&state_, 100);
  }

  int wait_count() const { return wait_count_.load(); }

 private:
  int state_ = 0;
  std::atomic<int> wait_count_{0};
};

int main() {
  FutexMutex mutex;

  mutex.lock();

  std::vector<std::thread> waiters;
  for (int i = 0; i < 4; ++i) {
    waiters.emplace_back([&, i] {
      mutex.lock();
      std::cout << "thread " << i << " entered critical section\n";
      std::this_thread::sleep_for(50ms);
      mutex.unlock_all();
    });
  }

  std::this_thread::sleep_for(300ms);
  std::cout << "main: wake all waiters\n";
  mutex.unlock_all();

  for (auto& waiter : waiters) {
    waiter.join();
  }

  std::cout << "futex_wait calls: " << mutex.wait_count() << "\n";
}
