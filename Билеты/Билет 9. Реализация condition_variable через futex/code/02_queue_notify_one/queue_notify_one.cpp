#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <atomic>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

int futex_wait(int* addr, int expected) {
  return syscall(SYS_futex, addr, FUTEX_WAIT_PRIVATE, expected, nullptr,
                 nullptr, 0);
}

int futex_wake(int* addr, int count) {
  return syscall(SYS_futex, addr, FUTEX_WAKE_PRIVATE, count, nullptr, nullptr,
                 0);
}

class FutexCondVar {
 public:
  void wait(std::mutex& mutex) {
    std::atomic_ref<int> seq(seq_);
    int snapshot = seq.load(std::memory_order_relaxed);

    mutex.unlock();
    futex_wait(&seq_, snapshot);
    mutex.lock();
  }

  template <typename Pred>
  void wait(std::mutex& mutex, Pred pred) {
    while (!pred()) {
      wait(mutex);
    }
  }

  void notify_one() {
    std::atomic_ref<int> seq(seq_);
    seq.fetch_add(1, std::memory_order_release);
    futex_wake(&seq_, 1);
  }

 private:
  int seq_ = 0;
};

int main() {
  std::mutex mutex;
  FutexCondVar cv;
  std::queue<int> queue;

  std::thread consumer([&] {
    mutex.lock();

    cv.wait(mutex, [&] { return !queue.empty(); });

    int value = queue.front();
    queue.pop();

    mutex.unlock();

    std::cout << "consumer got " << value << "\n";
  });

  std::thread producer([&] {
    mutex.lock();
    queue.push(42);
    mutex.unlock();

    cv.notify_one();
  });

  producer.join();
  consumer.join();
}
