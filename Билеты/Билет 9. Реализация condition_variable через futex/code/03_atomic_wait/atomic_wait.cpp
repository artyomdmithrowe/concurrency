#include <atomic>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

int main() {
  std::atomic<int> seq = 0;

  std::thread waiter([&] {
    int snapshot = seq.load();
    std::cout << "waiter: atomic_wait on seq = " << snapshot << "\n";

    seq.wait(snapshot);

    std::cout << "waiter: woke up, seq = " << seq.load() << "\n";
  });

  std::this_thread::sleep_for(300ms);

  seq.fetch_add(1);
  seq.notify_one();

  waiter.join();
}
