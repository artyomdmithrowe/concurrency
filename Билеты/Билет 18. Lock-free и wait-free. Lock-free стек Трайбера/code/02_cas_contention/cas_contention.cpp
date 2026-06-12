#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

constexpr int kThreads = 8;
constexpr int kIterations = 200'000;

int main() {
  std::atomic<int> counter{0};
  std::atomic<long long> cas_failures{0};
  std::vector<std::thread> threads;

  for (int id = 0; id < kThreads; ++id) {
    threads.emplace_back([&] {
      for (int i = 0; i < kIterations; ++i) {
        int old_value = counter.load(std::memory_order_relaxed);

        while (!counter.compare_exchange_weak(old_value, old_value + 1,
                                              std::memory_order_relaxed)) {
          ++cas_failures;
        }
      }
    });
  }

  for (std::thread& thread : threads) {
    thread.join();
  }

  std::cout << "expected counter = " << kThreads * kIterations << '\n';
  std::cout << "actual counter   = " << counter.load() << '\n';
  std::cout << "CAS failures     = " << cas_failures.load() << '\n';
}
