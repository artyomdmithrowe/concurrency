#include <atomic>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

int main() {
  std::atomic<int> balance = 100;
  std::atomic<int> ready = 0;
  std::atomic<bool> start = false;

  auto withdraw = [&](const char* name) {
    ready.fetch_add(1);
    while (!start.load()) {
    }

    int observed = balance.load();
    if (observed >= 80) {
      std::cout << name << " saw balance " << observed << "\n";
      std::this_thread::sleep_for(50ms);
      balance.fetch_sub(80);
      std::cout << name << " withdrew 80\n";
    }
  };

  std::thread first(withdraw, "first");
  std::thread second(withdraw, "second");

  while (ready.load() != 2) {
  }
  start.store(true);

  first.join();
  second.join();

  std::cout << "final balance: " << balance.load() << "\n";
}
