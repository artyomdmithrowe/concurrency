#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPoolExecutor {
 public:
  explicit ThreadPoolExecutor(std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
      workers_.emplace_back([this] { worker_loop(); });
    }
  }

  ~ThreadPoolExecutor() {
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

  void Execute(std::function<void()> task) {
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

class ManualExecutor {
 public:
  void Execute(std::function<void()> task) {
    std::lock_guard<std::mutex> lock(m_);
    tasks_.push(std::move(task));
  }

  bool DrainOne() {
    std::function<void()> task;

    {
      std::lock_guard<std::mutex> lock(m_);

      if (tasks_.empty()) {
        return false;
      }

      task = std::move(tasks_.front());
      tasks_.pop();
    }

    task();
    return true;
  }

 private:
  std::queue<std::function<void()>> tasks_;
  std::mutex m_;
};

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
  ThreadPoolExecutor cpu_executor(2);
  ManualExecutor io_executor;

  auto response_written = std::make_shared<std::promise<void>>();
  std::future<void> done = response_written->get_future();

  int request = 1'000'003;

  std::cout << "I/O thread received request: " << request << '\n';

  cpu_executor.Execute([request, &io_executor, response_written] {
    bool prime = is_prime(request);

    io_executor.Execute([request, prime, response_written] {
      std::cout << "I/O thread writes response: " << request << " is "
                << (prime ? "prime" : "not prime") << '\n';
      response_written->set_value();
    });
  });

  while (done.wait_for(std::chrono::milliseconds(0)) !=
         std::future_status::ready) {
    if (!io_executor.DrainOne()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  done.get();
}
