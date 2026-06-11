#include <atomic>
#include <cstdint>
#include <iostream>
#include <thread>

void compiler_barrier() { asm volatile("" ::: "memory"); }

int main() {
  constexpr uint64_t iterations = 1'000'000;
  uint64_t c00 = 0;
  uint64_t c01 = 0;
  uint64_t c10 = 0;
  uint64_t c11 = 0;

  volatile int x = 0;
  volatile int y = 0;
  volatile int a = 0;
  volatile int b = 0;

  std::atomic<uint64_t> phase{0};
  std::atomic<int> done{0};
  std::atomic<bool> stop{false};

  std::thread t1([&] {
    uint64_t local_phase = 0;

    while (!stop.load(std::memory_order_relaxed)) {
      uint64_t current = phase.load(std::memory_order_acquire);
      if (current == local_phase) {
        continue;
      }

      local_phase = current;

      x = 1;
      compiler_barrier();
      a = y;

      done.fetch_add(1, std::memory_order_release);
    }
  });

  std::thread t2([&] {
    uint64_t local_phase = 0;

    while (!stop.load(std::memory_order_relaxed)) {
      uint64_t current = phase.load(std::memory_order_acquire);
      if (current == local_phase) {
        continue;
      }

      local_phase = current;

      y = 1;
      compiler_barrier();
      b = x;

      done.fetch_add(1, std::memory_order_release);
    }
  });

  for (uint64_t i = 0; i < iterations; ++i) {
    x = 0;
    y = 0;
    a = 0;
    b = 0;

    done.store(0, std::memory_order_relaxed);
    phase.store(i + 1, std::memory_order_release);

    while (done.load(std::memory_order_acquire) != 2) {
    }

    if (a == 0 && b == 0) {
      ++c00;
    } else if (a == 0 && b == 1) {
      ++c01;
    } else if (a == 1 && b == 0) {
      ++c10;
    } else {
      ++c11;
    }
  }

  stop.store(true, std::memory_order_relaxed);
  phase.fetch_add(1, std::memory_order_release);
  t1.join();
  t2.join();

  std::cout << "(0,0)=" << c00 << " (0,1)=" << c01 << " (1,0)=" << c10
            << " (1,1)=" << c11 << "\n";
}
