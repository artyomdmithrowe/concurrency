#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

int main() {
  constexpr int kThreads = 4;
  constexpr int kIterations = 500'000;

  std::atomic<int> counter{0};
  std::vector<std::thread> threads;

  for (int id = 0; id < kThreads; ++id) {
    threads.emplace_back([&] {
      for (int i = 0; i < kIterations; ++i) {
        counter.fetch_add(1, std::memory_order_relaxed);
      }
    });
  }

  for (std::thread& thread : threads) {
    thread.join();
  }

  std::cout << "expected = " << kThreads * kIterations << '\n';
  std::cout << "actual   = " << counter.load(std::memory_order_relaxed) << '\n';
  std::cout << "relaxed is enough for this standalone counter\n";
}
