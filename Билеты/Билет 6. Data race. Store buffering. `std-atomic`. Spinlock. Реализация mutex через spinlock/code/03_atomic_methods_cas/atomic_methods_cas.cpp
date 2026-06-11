#include <atomic>
#include <iostream>

int main() {
  std::atomic<int> x{0};

  x.store(10);
  std::cout << "load after store: " << x.load() << "\n";

  int old = x.exchange(20);
  std::cout << "exchange old=" << old << " current=" << x.load() << "\n";

  old = x.fetch_add(5);
  std::cout << "fetch_add old=" << old << " current=" << x.load() << "\n";

  int expected = 25;
  bool ok = x.compare_exchange_strong(expected, 100);
  std::cout << "CAS #1 ok=" << ok << " expected=" << expected
            << " current=" << x.load() << "\n";

  expected = 25;
  ok = x.compare_exchange_strong(expected, 200);
  std::cout << "CAS #2 ok=" << ok << " expected=" << expected
            << " current=" << x.load() << "\n";
}
