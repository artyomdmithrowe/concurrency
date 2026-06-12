#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

std::future<int> delayed_value(std::vector<std::thread>& producers, int value,
                               std::chrono::milliseconds delay) {
  std::promise<int> promise;
  std::future<int> future = promise.get_future();

  producers.emplace_back(
      [promise = std::move(promise), value, delay]() mutable {
        std::this_thread::sleep_for(delay);
        promise.set_value(value);
      });

  return future;
}

std::vector<int> collect_all(std::vector<std::future<int>> futures) {
  std::vector<int> results;
  results.reserve(futures.size());

  for (auto& future : futures) {
    results.push_back(future.get());
  }

  return results;
}

int collect_any(std::vector<std::future<int>> futures) {
  while (true) {
    for (auto& future : futures) {
      if (future.valid() && future.wait_for(0ms) == std::future_status::ready) {
        return future.get();
      }
    }

    std::this_thread::sleep_for(10ms);
  }
}

int main() {
  {
    std::vector<std::thread> producers;
    std::vector<std::future<int>> futures;
    futures.push_back(delayed_value(producers, 10, 300ms));
    futures.push_back(delayed_value(producers, 20, 100ms));
    futures.push_back(delayed_value(producers, 30, 200ms));

    std::vector<int> results = collect_all(std::move(futures));

    std::cout << "collect_all:";
    for (int value : results) {
      std::cout << ' ' << value;
    }
    std::cout << '\n';

    for (auto& producer : producers) {
      producer.join();
    }
  }

  {
    std::vector<std::thread> producers;
    std::vector<std::future<int>> futures;
    futures.push_back(delayed_value(producers, 100, 300ms));
    futures.push_back(delayed_value(producers, 200, 100ms));
    futures.push_back(delayed_value(producers, 300, 200ms));

    std::cout << "collect_any: " << collect_any(std::move(futures)) << '\n';

    for (auto& producer : producers) {
      producer.join();
    }
  }
}
