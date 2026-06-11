#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <atomic>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

int futex_wait(int* addr, int expected) {
  return syscall(SYS_futex, addr, FUTEX_WAIT_PRIVATE, expected, nullptr,
                 nullptr, 0);
}

int futex_wake(int* addr, int count) {
  return syscall(SYS_futex, addr, FUTEX_WAKE_PRIVATE, count, nullptr, nullptr,
                 0);
}

int main() {
  int seq = 0;
  std::atomic_ref<int> atomic_seq(seq);

  std::thread waiter([&] {
    int snapshot = atomic_seq.load(std::memory_order_relaxed);
    std::cout << "waiter: snapshot seq = " << snapshot << "\n";

    futex_wait(&seq, snapshot);

    std::cout << "waiter: woke up, seq = " << atomic_seq.load() << "\n";
  });

  std::this_thread::sleep_for(300ms);

  int old = atomic_seq.fetch_add(1, std::memory_order_release);
  int woken = futex_wake(&seq, 1);

  std::cout << "main: seq " << old << " -> " << old + 1 << "\n";
  std::cout << "main: futex_wake woke " << woken << " waiter\n";

  waiter.join();
}
