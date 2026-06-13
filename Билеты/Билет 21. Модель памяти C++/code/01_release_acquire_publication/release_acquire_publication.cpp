#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

struct Data {
  int answer = 0;
  std::vector<int> values;
};

int main() {
  Data data;
  std::atomic<bool> ready{false};

  std::thread producer([&] {
    data.answer = 42;
    data.values = {1, 2, 3};

    ready.store(true, std::memory_order_release);
  });

  std::thread consumer([&] {
    while (!ready.load(std::memory_order_acquire)) {
    }

    assert(data.answer == 42);
    assert(data.values.size() == 3);

    std::cout << "consumer sees answer = " << data.answer << '\n';
    std::cout << "consumer sees values.size = " << data.values.size() << '\n';
  });

  producer.join();
  consumer.join();
}
