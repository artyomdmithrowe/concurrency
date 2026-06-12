#include <algorithm>
#include <atomic>
#include <chrono>
#include <iostream>
#include <new>
#include <thread>
#include <vector>

constexpr long long kIterations = 2'000'000;

struct alignas(std::hardware_destructive_interference_size) Counter {
  std::atomic<long long> value{0};
};

long long NowMs(std::chrono::steady_clock::time_point started) {
  auto finished = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(finished -
                                                               started)
      .count();
}

long long RunGlobalCounter(int thread_count) {
  std::atomic<long long> global{0};
  std::vector<std::thread> threads;

  auto started = std::chrono::steady_clock::now();

  for (int id = 0; id < thread_count; ++id) {
    threads.emplace_back([&] {
      for (long long i = 0; i < kIterations; ++i) {
        global.fetch_add(1, std::memory_order_relaxed);
      }
    });
  }

  for (std::thread& thread : threads) {
    thread.join();
  }

  std::cout << "global sum = " << global.load(std::memory_order_relaxed)
            << '\n';
  return NowMs(started);
}

long long RunPerThreadCounters(int thread_count) {
  std::vector<Counter> counters(thread_count);
  std::vector<std::thread> threads;

  auto started = std::chrono::steady_clock::now();

  for (int id = 0; id < thread_count; ++id) {
    threads.emplace_back([&, id] {
      for (long long i = 0; i < kIterations; ++i) {
        counters[id].value.fetch_add(1, std::memory_order_relaxed);
      }
    });
  }

  for (std::thread& thread : threads) {
    thread.join();
  }

  long long total = 0;
  for (Counter& counter : counters) {
    total += counter.value.load(std::memory_order_relaxed);
  }

  std::cout << "per-thread sum = " << total << '\n';
  return NowMs(started);
}

int main() {
  int hardware_threads = static_cast<int>(std::thread::hardware_concurrency());
  int thread_count = std::clamp(hardware_threads, 2, 8);

  std::cout << "thread_count = " << thread_count << '\n';
  std::cout << "iterations per thread = " << kIterations << "\n\n";

  long long global_ms = RunGlobalCounter(thread_count);
  long long per_thread_ms = RunPerThreadCounters(thread_count);

  std::cout << "\nglobal counter ms      = " << global_ms << '\n';
  std::cout << "per-thread counters ms = " << per_thread_ms << '\n';
}
