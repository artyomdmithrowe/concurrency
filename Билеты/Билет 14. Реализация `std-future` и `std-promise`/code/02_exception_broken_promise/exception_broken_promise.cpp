#include <condition_variable>
#include <exception>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <thread>

struct BrokenPromise : std::runtime_error {
  BrokenPromise() : std::runtime_error("broken promise") {}
};

struct PromiseAlreadySatisfied : std::runtime_error {
  PromiseAlreadySatisfied() : std::runtime_error("promise already satisfied") {}
};

template <typename T>
struct SharedState {
  std::mutex m;
  std::condition_variable cv;
  bool ready = false;
  std::optional<T> value;
  std::exception_ptr exception;
};

template <typename T>
class Future {
 public:
  explicit Future(std::shared_ptr<SharedState<T>> state)
      : state_(std::move(state)) {}

  T get() {
    std::unique_lock<std::mutex> lock(state_->m);
    state_->cv.wait(lock, [this] { return state_->ready; });

    if (state_->exception) {
      std::rethrow_exception(state_->exception);
    }

    return std::move(*state_->value);
  }

 private:
  std::shared_ptr<SharedState<T>> state_;
};

template <typename T>
class Promise {
 public:
  explicit Promise(std::shared_ptr<SharedState<T>> state)
      : state_(std::move(state)) {}

  Promise(const Promise&) = delete;
  Promise& operator=(const Promise&) = delete;

  Promise(Promise&&) noexcept = default;
  Promise& operator=(Promise&&) noexcept = default;

  ~Promise() {
    if (!state_) {
      return;
    }

    bool should_notify = false;
    {
      std::lock_guard<std::mutex> lock(state_->m);
      if (!state_->ready) {
        state_->exception = std::make_exception_ptr(BrokenPromise());
        state_->ready = true;
        should_notify = true;
      }
    }

    if (should_notify) {
      state_->cv.notify_all();
    }
  }

  void set_value(T value) {
    set_result([&] { state_->value = std::move(value); });
  }

  void set_exception(std::exception_ptr exception) {
    set_result([&] { state_->exception = std::move(exception); });
  }

 private:
  template <typename F>
  void set_result(F fill_result) {
    {
      std::lock_guard<std::mutex> lock(state_->m);
      if (state_->ready) {
        throw PromiseAlreadySatisfied();
      }

      fill_result();
      state_->ready = true;
    }

    state_->cv.notify_all();
  }

  std::shared_ptr<SharedState<T>> state_;
};

template <typename T>
std::pair<Future<T>, Promise<T>> make_contract() {
  auto state = std::make_shared<SharedState<T>>();
  return {Future<T>(state), Promise<T>(state)};
}

int main() {
  {
    auto [future, promise] = make_contract<int>();

    std::thread producer([promise = std::move(promise)]() mutable {
      try {
        throw std::runtime_error("worker failed");
      } catch (...) {
        promise.set_exception(std::current_exception());
      }
    });

    try {
      int value = future.get();
      std::cout << "future.get(): " << value << '\n';
    } catch (const std::exception& e) {
      std::cout << "future.get() rethrew: " << e.what() << '\n';
    }

    producer.join();
  }

  {
    std::optional<Future<int>> future;

    {
      auto [f, promise] = make_contract<int>();
      future.emplace(std::move(f));
      (void)promise;
    }

    try {
      int value = future->get();
      std::cout << "future.get(): " << value << '\n';
    } catch (const std::exception& e) {
      std::cout << "future.get() after promise destruction: " << e.what()
                << '\n';
    }
  }
}
