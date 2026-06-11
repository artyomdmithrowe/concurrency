#include <iostream>
#include <thread>
#include <vector>

class BadSemaphore {
public:
  explicit BadSemaphore(int initial) : count_(initial) {}

  void acquire() {
    while (count_ == 0) {
      std::this_thread::yield();
    }

    --count_;
  }

  void release() { ++count_; }

private:
  int count_;
};

int main() {
  BadSemaphore sem(2);
  std::vector<std::thread> threads;

  for (int i = 0; i < 6; ++i) {
    threads.emplace_back([&, i] {
      sem.acquire();
      std::cout << "entered #" << i << "\n";
      sem.release();
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}
