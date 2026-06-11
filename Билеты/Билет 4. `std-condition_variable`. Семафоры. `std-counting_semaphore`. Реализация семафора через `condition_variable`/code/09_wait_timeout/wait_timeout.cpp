#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

int main() {
  std::mutex m;
  std::condition_variable cv;
  bool ready = false;

  std::thread notifier([&] {
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    {
      std::lock_guard<std::mutex> lock(m);
      ready = true;
    }

    cv.notify_one();
  });

  std::unique_lock<std::mutex> lock(m);

  bool ok = cv.wait_for(lock, std::chrono::seconds(1), [&] { return ready; });

  std::cout << "wait result: " << (ok ? "ready" : "timeout") << "\n";

  //   lock.unlock();
  notifier.join();
}
