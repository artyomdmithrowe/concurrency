#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
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

  bool submit(std::function<void()> task) {
    {
      std::lock_guard<std::mutex> lock(m_);
      if (stop_) {
        return false;
      }
      tasks_.push(std::move(task));
    }
    cv_.notify_one();
    return true;
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

      task();
    }
  }

  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> tasks_;
  std::mutex m_;
  std::condition_variable cv_;
  bool stop_ = false;
};

template <typename F>
auto submit_with_future(ThreadPool& pool, F f)
    -> std::future<std::invoke_result_t<F>> {
  using R = std::invoke_result_t<F>;

  auto task = std::make_shared<std::packaged_task<R()>>(std::move(f));
  std::future<R> future = task->get_future();

  pool.submit([task] { (*task)(); });

  return future;
}

int main() {
  ThreadPool pool(2);

  auto future = submit_with_future(pool, [] { return 42; });

  std::cout << "result: " << future.get() << "\n";
}
