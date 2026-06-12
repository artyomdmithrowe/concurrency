#include <chrono>
#include <future>
#include <iostream>
#include <thread>

int main() {
  std::promise<int> promise;
  std::future<int> future = promise.get_future();

  std::thread producer([promise = std::move(promise)]() mutable {
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    promise.set_value(42);
  });

  std::cout << "future exists, but value is not ready yet\n";
  int value = future.get();
  std::cout << "future.get() returned " << value << '\n';

  producer.join();
}
