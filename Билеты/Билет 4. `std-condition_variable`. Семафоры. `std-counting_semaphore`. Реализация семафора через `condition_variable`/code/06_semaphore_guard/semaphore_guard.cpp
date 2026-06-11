#include <chrono>
#include <iostream>
#include <semaphore>
#include <thread>
#include <vector>

template <std::ptrdiff_t Max>
class SemaphoreGuard {
 public:
  explicit SemaphoreGuard(std::counting_semaphore<Max>& sem) : sem_(sem) {
    sem_.acquire();
  }

  ~SemaphoreGuard() { sem_.release(); }

  SemaphoreGuard(const SemaphoreGuard&) = delete;
  SemaphoreGuard& operator=(const SemaphoreGuard&) = delete;

 private:
  std::counting_semaphore<Max>& sem_;
};

int main() {
  std::counting_semaphore<2> slots(2);
  std::vector<std::thread> threads;

  for (int i = 0; i < 6; ++i) {
    threads.emplace_back([&, i] {
      SemaphoreGuard guard(slots);

      std::cout << "inside guarded section #" << i << "\n";
      std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }
}
