#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

class ThreadPool {
 public:
  explicit ThreadPool(size_t n) {
    for (size_t i = 0; i < n; ++i) {
      workers_.emplace_back([this] { worker_loop(); });
    }
  }

  ~ThreadPool() {
    {
      std::lock_guard<std::mutex> lock(m_);
      stop_ = true;
    }
    cv_.notify_all();

    for (auto& worker : workers_) {
      worker.join();
    }
  }

  void submit(std::function<void()> task) {
    {
      std::lock_guard<std::mutex> lock(m_);
      tasks_.push(std::move(task));
    }
    cv_.notify_one();
  }

 private:
  void worker_loop() {
    while (true) {
      std::function<void()> task;

      {
        std::unique_lock<std::mutex> lock(m_);
        cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

        if (stop_ && tasks_.empty()) {
          return;
        }

        task = std::move(tasks_.front());
        tasks_.pop();
      }

      try {
        task();
      } catch (const std::exception& error) {
        std::cerr << "task exception: " << error.what() << "\n";
      } catch (...) {
        std::cerr << "unknown task exception\n";
      }
    }
  }

  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> tasks_;
  std::mutex m_;
  std::condition_variable cv_;
  bool stop_ = false;
};

int main() {
  ThreadPool pool(2);

  pool.submit([] { std::cout << "normal task\n"; });

  pool.submit([] { throw std::runtime_error("boom"); });

  pool.submit([] { std::cout << "worker survived exception\n"; });
}
