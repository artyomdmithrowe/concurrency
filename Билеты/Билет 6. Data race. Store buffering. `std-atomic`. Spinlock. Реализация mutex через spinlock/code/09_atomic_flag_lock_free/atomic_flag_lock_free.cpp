#include <atomic>
#include <iostream>
#include <thread>

class FlagSpinLock {
 public:
  void lock() {
    while (flag_.test_and_set(std::memory_order_acquire)) {
      std::this_thread::yield();
    }
  }

  void unlock() { flag_.clear(std::memory_order_release); }

 private:
  std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

struct Big {
  long long a;
  long long b;
  long long c;
  long long d;
};

int main() {
  FlagSpinLock lock;
  int value = 0;

  std::thread t1([&] {
    lock.lock();
    ++value;
    lock.unlock();
  });

  std::thread t2([&] {
    lock.lock();
    ++value;
    lock.unlock();
  });

  t1.join();
  t2.join();

  std::atomic<int> ai;
  std::atomic<long long> all;
  std::atomic<Big> big;

  std::cout << "value = " << value << "\n";
  std::cout << "atomic<int>.is_lock_free(): " << ai.is_lock_free() << "\n";
  std::cout << "atomic<long long>.is_lock_free(): " << all.is_lock_free()
            << "\n";
  std::cout << "atomic<Big>.is_lock_free(): " << big.is_lock_free() << "\n";
}
