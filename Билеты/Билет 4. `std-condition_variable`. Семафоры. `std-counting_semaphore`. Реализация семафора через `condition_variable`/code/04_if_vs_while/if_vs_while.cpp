#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

class IfSemaphore {
public:
  explicit IfSemaphore(int initial) : count_(initial) {}

  void acquire_bad() {
    std::unique_lock<std::mutex> lock(m_);

    if (count_ == 0) {
      cv_.wait(lock);
    }

    --count_;
  }

  void acquire_good() {
    std::unique_lock<std::mutex> lock(m_);

    while (count_ == 0) {
      cv_.wait(lock);
    }

    --count_;
  }

  void release() {
    {
      std::lock_guard<std::mutex> lock(m_);
      ++count_;
    }

    cv_.notify_one();
  }

  void fake_notify_without_resource() { cv_.notify_all(); }

  int count() const {
    std::lock_guard<std::mutex> lock(m_);
    return count_;
  }

private:
  mutable std::mutex m_;
  std::condition_variable cv_;
  int count_;
};

int main() {
  IfSemaphore sem(0);

  std::thread bad([&] {
    sem.acquire_bad();
    std::cout << "bad acquire passed after notify without resource\n";
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  sem.fake_notify_without_resource();
  bad.join();

  std::cout << "count after bad acquire: " << sem.count() << "\n";
}
