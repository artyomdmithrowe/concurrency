#include <atomic>
#include <cstdint>
#include <iostream>
#include <thread>

int main() {
  constexpr std::uint64_t kAttempts = 1'000'000;

  std::atomic<int> x{0};
  std::atomic<int> y{0};

  int r1 = -1;
  int r2 = -1;

  std::uint64_t both_zero = 0;
  std::uint64_t only_first_zero = 0;
  std::uint64_t only_second_zero = 0;
  std::uint64_t both_one = 0;

  std::atomic<std::uint64_t> phase{0};
  std::atomic<int> done{0};
  std::atomic<bool> stop{false};

  std::thread first([&] {
    std::uint64_t local_phase = 0;

    while (!stop.load(std::memory_order_relaxed)) {
      std::uint64_t current = phase.load(std::memory_order_acquire);
      if (current == local_phase) {
        continue;
      }

      local_phase = current;

      x.store(1, std::memory_order_relaxed);
      r1 = y.load(std::memory_order_relaxed);

      done.fetch_add(1, std::memory_order_release);
    }
  });

  std::thread second([&] {
    std::uint64_t local_phase = 0;

    while (!stop.load(std::memory_order_relaxed)) {
      std::uint64_t current = phase.load(std::memory_order_acquire);
      if (current == local_phase) {
        continue;
      }

      local_phase = current;

      y.store(1, std::memory_order_relaxed);
      r2 = x.load(std::memory_order_relaxed);

      done.fetch_add(1, std::memory_order_release);
    }
  });

  for (std::uint64_t attempt = 0; attempt < kAttempts; ++attempt) {
    x.store(0, std::memory_order_relaxed);
    y.store(0, std::memory_order_relaxed);
    r1 = -1;
    r2 = -1;

    done.store(0, std::memory_order_relaxed);
    phase.store(attempt + 1, std::memory_order_release);

    while (done.load(std::memory_order_acquire) != 2) {
    }

    if (r1 == 0 && r2 == 0) {
      ++both_zero;
    } else if (r1 == 0 && r2 == 1) {
      ++only_first_zero;
    } else if (r1 == 1 && r2 == 0) {
      ++only_second_zero;
    } else {
      ++both_one;
    }
  }

  stop.store(true, std::memory_order_relaxed);
  phase.fetch_add(1, std::memory_order_release);

  first.join();
  second.join();

  std::cout << "attempts = " << kAttempts << '\n';
  std::cout << "(r1=0, r2=0) = " << both_zero << '\n';
  std::cout << "(r1=0, r2=1) = " << only_first_zero << '\n';
  std::cout << "(r1=1, r2=0) = " << only_second_zero << '\n';
  std::cout << "(r1=1, r2=1) = " << both_one << '\n';
}
