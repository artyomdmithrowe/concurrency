#include <atomic>
#include <chrono>
#include <iostream>
#include <new>
#include <thread>

constexpr long long kIterations = 10'000'000;

struct Counter {
  std::atomic<long long> value{0};
};

struct alignas(std::hardware_destructive_interference_size) PaddedCounter {
  std::atomic<long long> value{0};
};

template <class Counters>
long long RunTwoCounters(Counters& counters) {
  auto started = std::chrono::steady_clock::now();

  std::thread first([&] {
    for (long long i = 0; i < kIterations; ++i) {
      counters[0].value.fetch_add(1, std::memory_order_relaxed);
    }
  });

  std::thread second([&] {
    for (long long i = 0; i < kIterations; ++i) {
      counters[1].value.fetch_add(1, std::memory_order_relaxed);
    }
  });

  first.join();
  second.join();

  auto finished = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(finished -
                                                               started)
      .count();
}

int main() {
  Counter adjacent[2];
  PaddedCounter padded[2];

  long long adjacent_ms = RunTwoCounters(adjacent);
  long long padded_ms = RunTwoCounters(padded);

  std::cout << "iterations per thread = " << kIterations << "\n\n";

  std::cout << "adjacent counters ms = " << adjacent_ms << '\n';
  std::cout << "padded counters ms   = " << padded_ms << '\n';
  std::cout << "sum adjacent = "
            << adjacent[0].value.load(std::memory_order_relaxed) +
                   adjacent[1].value.load(std::memory_order_relaxed)
            << '\n';
  std::cout << "sum padded   = "
            << padded[0].value.load(std::memory_order_relaxed) +
                   padded[1].value.load(std::memory_order_relaxed)
            << '\n';
}
