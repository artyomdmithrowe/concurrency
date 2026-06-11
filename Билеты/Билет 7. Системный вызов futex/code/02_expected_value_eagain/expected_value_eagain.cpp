#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <atomic>
#include <cerrno>
#include <cstring>
#include <iostream>

int futex_wait(int* addr, int expected) {
  return syscall(SYS_futex, addr, FUTEX_WAIT_PRIVATE, expected, nullptr,
                 nullptr, 0);
}

int main() {
  int state = 0;
  std::atomic_ref<int> atomic_state(state);

  atomic_state.store(1, std::memory_order_release);

  errno = 0;
  int rc = futex_wait(&state, 0);

  std::cout << "state = " << atomic_state.load() << "\n";
  std::cout << "futex_wait expected 0 returned " << rc << "\n";
  std::cout << "errno = " << errno << " (" << std::strerror(errno) << ")\n";
}
