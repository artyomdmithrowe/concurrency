#include <chrono>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <vector>

class PrimeCache {
public:
  bool get(long long number) {
    {
      std::shared_lock lock(mutex_);

      auto it = cache_.find(number);
      if (it != cache_.end()) {
        return it->second;
      }
    }

    bool result = is_prime(number);

    {
      std::unique_lock lock(mutex_);

      auto [it, inserted] = cache_.emplace(number, result);
      return it->second;
    }
  }

  size_t size() const {
    std::shared_lock lock(mutex_);
    return cache_.size();
  }

private:
  static bool is_prime(long long n) {
    if (n <= 1) {
      return false;
    }
    if (n == 2) {
      return true;
    }
    if (n % 2 == 0) {
      return false;
    }
    for (long long d = 3; d * d <= n; d += 2) {
      if (n % d == 0) {
        return false;
      }
    }
    return true;
  }

  mutable std::shared_mutex mutex_;
  std::unordered_map<long long, bool> cache_;
};

int main() {
  constexpr int threads_count = 8;
  constexpr long long begin = 1'000'000;
  constexpr long long end = 1'020'000;

  PrimeCache cache;

  auto run = [&] {
    std::vector<std::thread> threads;

    for (int i = 0; i < threads_count; ++i) {
      threads.emplace_back([&] {
        for (long long n = begin; n < end; ++n) {
          cache.get(n);
        }
      });
    }

    for (auto &thread : threads) {
      thread.join();
    }
  };

  auto measure_ms = [&](const char *name) {
    auto start = std::chrono::steady_clock::now();
    run();
    auto finish = std::chrono::steady_clock::now();

    auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(finish - start)
            .count();

    std::cout << name << ": " << ms << " ms\n";
  };

  measure_ms("cold fill");
  measure_ms("hot reads");

  std::cout << "cache size: " << cache.size() << "\n";
}
