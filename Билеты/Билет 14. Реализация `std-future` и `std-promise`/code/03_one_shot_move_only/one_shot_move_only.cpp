#include <future>
#include <iostream>
#include <memory>

int main() {
  std::promise<std::unique_ptr<int>> promise;
  std::future<std::unique_ptr<int>> future = promise.get_future();

  promise.set_value(std::make_unique<int>(123));

  std::unique_ptr<int> ptr = future.get();
  std::cout << "first get moved value: " << *ptr << '\n';

  try {
    future.get();
  } catch (const std::future_error& e) {
    std::cout << "second get failed: " << e.what() << '\n';
  }

  std::promise<int> p;
  std::future<int> f1 = p.get_future();
  std::future<int> f2 = std::move(f1);
  p.set_value(7);

  std::cout << "moved future value: " << f2.get() << '\n';
  std::cout << "old future valid: " << std::boolalpha << f1.valid() << '\n';
}
