#include <iostream>
#include <thread>
#include <vector>

int main() {
  constexpr int threads_count = 8;
  constexpr int iterations = 500'000;

  int counter = 0;
  std::vector<std::thread> threads;

  for (int i = 0; i < threads_count; ++i) {
    threads.emplace_back([&] {
      for (int j = 0; j < iterations; ++j) {
        ++counter;
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  std::cout << "expected: " << threads_count * iterations << "\n";
  std::cout << "actual:   " << counter << "\n";
}
