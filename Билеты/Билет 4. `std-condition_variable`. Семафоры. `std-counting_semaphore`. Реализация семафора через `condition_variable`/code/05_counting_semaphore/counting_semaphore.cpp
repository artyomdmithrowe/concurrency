#include <chrono>
#include <iostream>
#include <semaphore>
#include <thread>
#include <vector>

int main() {
  std::counting_semaphore<3> slots(3);
  std::vector<std::thread> threads;

  for (int i = 0; i < 8; ++i) {
    threads.emplace_back([&, i] {
      slots.acquire();

      std::cout << "using resource #" << i << "\n";
      std::this_thread::sleep_for(std::chrono::milliseconds(2000));

      slots.release();
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}
