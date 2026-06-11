#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <atomic>
#include <cerrno>
#include <climits>
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
  int state = 0;
  std::atomic_ref<int> atomic_state(state);

  std::thread waiter([&] {
    std::cout << "waiter: state == 0, going to futex_wait\n";
    int rc = futex_wait(&state, 0);
    std::cout << "waiter: futex_wait returned " << rc
              << ", state = " << atomic_state.load() << "\n";
  });

  std::this_thread::sleep_for(3000ms);
  atomic_state.store(1, std::memory_order_release);
  int woken = futex_wake(&state, 1);

  std::cout << "main: state = 1, futex_wake woke " << woken << " waiter\n";
  waiter.join();
}
