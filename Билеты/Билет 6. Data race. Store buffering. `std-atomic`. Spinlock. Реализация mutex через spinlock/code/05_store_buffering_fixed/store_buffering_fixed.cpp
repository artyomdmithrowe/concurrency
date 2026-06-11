#include <atomic>
#include <cstdint>
#include <iostream>
#include <thread>

int main() {
  constexpr uint64_t iterations = 20'000;
  uint64_t c00 = 0;
  uint64_t c01 = 0;
  uint64_t c10 = 0;
  uint64_t c11 = 0;

  std::atomic<int> x = 0;
  std::atomic<int> y = 0;

  for (uint64_t i = 0; i < iterations; ++i) {
    int a = 0;
    int b = 0;
    std::atomic<bool> start = false;

    x.store(0, std::memory_order_seq_cst);
    y.store(0, std::memory_order_seq_cst);

    std::thread t1([&] {
      while (!start.load(std::memory_order_acquire)) {
      }

      x.store(1, std::memory_order_seq_cst);
      a = y.load(std::memory_order_seq_cst);
    });

    std::thread t2([&] {
      while (!start.load(std::memory_order_acquire)) {
      }

      y.store(1, std::memory_order_seq_cst);
      b = x.load(std::memory_order_seq_cst);
    });

    start.store(true, std::memory_order_release);

    t1.join();
    t2.join();

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

  std::cout << "(0,0)=" << c00 << " (0,1)=" << c01 << " (1,0)=" << c10
            << " (1,1)=" << c11 << "\n";
}
