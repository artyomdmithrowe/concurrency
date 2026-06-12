#include <future>
#include <iostream>
#include <thread>

int main() {
  std::promise<int> promise;
  std::future<int> future = promise.get_future();

  promise.set_value(10);
  std::cout << "first future.get(): " << future.get() << '\n';

  try {
    int value = future.get();
    std::cout << "second future.get(): " << value << '\n';
  } catch (const std::future_error& e) {
    std::cout << "second future.get() failed: " << e.what() << '\n';
  }

  std::promise<int> shared_promise;
  std::shared_future<int> shared = shared_promise.get_future().share();
  shared_promise.set_value(99);

  std::thread t1(
      [shared] { std::cout << "thread 1 sees " << shared.get() << '\n'; });
  std::thread t2(
      [shared] { std::cout << "thread 2 sees " << shared.get() << '\n'; });

  t1.join();
  t2.join();

  std::cout << "main sees again " << shared.get() << '\n';
}
