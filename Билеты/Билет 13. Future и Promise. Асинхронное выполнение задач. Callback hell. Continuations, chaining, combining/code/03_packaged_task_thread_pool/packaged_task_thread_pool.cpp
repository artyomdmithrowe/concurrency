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
  explicit ThreadPool(std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
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
      if (worker.joinable()) {
        worker.join();
      }
    }
  }

  void submit_void(std::function<void()> task) {
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
auto submit(ThreadPool& pool, F f) -> std::future<std::invoke_result_t<F>> {
  using R = std::invoke_result_t<F>;

  auto task = std::make_shared<std::packaged_task<R()>>(std::move(f));
  std::future<R> future = task->get_future();

  pool.submit_void([task] { (*task)(); });
  return future;
}

bool is_prime(int n) {
  if (n < 2) {
    return false;
  }

  for (int d = 2; d * d <= n; ++d) {
    if (n % d == 0) {
      return false;
    }
  }

  return true;
}

int main() {
  ThreadPool pool(3);

  auto f1 = submit(pool, [] { return is_prime(1'000'003); });
  auto f2 = submit(pool, [] { return is_prime(1'000'005); });

  std::cout << "1000003 prime: " << std::boolalpha << f1.get() << '\n';
  std::cout << "1000005 prime: " << std::boolalpha << f2.get() << '\n';
}
