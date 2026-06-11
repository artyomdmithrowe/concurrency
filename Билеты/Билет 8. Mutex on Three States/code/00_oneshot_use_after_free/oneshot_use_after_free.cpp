#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

int futex_wake(int* addr, int count) {
  return syscall(SYS_futex, addr, FUTEX_WAKE_PRIVATE, count, nullptr, nullptr,
                 0);
}

std::atomic<bool> g_unlock_is_between_store_and_wake = false;
std::atomic<bool> g_may_finish_unlock = false;

class BadTwoStateMutex {
 public:
  void lock() {
    std::atomic_ref<int> state(state_);
    while (state.exchange(1, std::memory_order_acquire) != 0) {
    }
  }

  void unlock() {
    int* futex_word = &state_;
    std::atomic_ref<int> state(*futex_word);

    state.store(0, std::memory_order_release);
    g_unlock_is_between_store_and_wake.store(true, std::memory_order_release);

    while (!g_may_finish_unlock.load(std::memory_order_acquire)) {
      std::this_thread::yield();
    }

    volatile int read_after_possible_destroy = *futex_word;
    (void)read_after_possible_destroy;
    futex_wake(futex_word, 1);
  }

 private:
  int state_ = 0;
};

class OneShot {
 public:
  void call() {
    std::lock_guard<BadTwoStateMutex> guard(mutex_);

    if (!called_) {
      called_ = true;
      std::cout << "OneShot function is called\n";
    }
  }

 private:
  BadTwoStateMutex mutex_;
  bool called_ = false;
};

int main() {
  OneShot* one_shot = new OneShot;

  std::thread caller([&] { one_shot->call(); });

  while (!g_unlock_is_between_store_and_wake.load(std::memory_order_acquire)) {
    std::this_thread::yield();
  }

  std::cout << "main: mutex is already free, deleting OneShot\n";
  delete one_shot;

  g_may_finish_unlock.store(true, std::memory_order_release);
  caller.join();
}
