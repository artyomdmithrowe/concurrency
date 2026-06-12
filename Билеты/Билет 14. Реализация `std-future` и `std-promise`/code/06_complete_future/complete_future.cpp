#include <condition_variable>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>

struct FutureInvalid : std::runtime_error {
  FutureInvalid() : std::runtime_error("future is invalid") {}
};

struct PromiseAlreadySatisfied : std::runtime_error {
  PromiseAlreadySatisfied() : std::runtime_error("promise already satisfied") {}
};

struct BrokenPromise : std::runtime_error {
  BrokenPromise() : std::runtime_error("broken promise") {}
};

template <typename T>
struct Result {
  std::optional<T> value;
  std::exception_ptr error;

  static Result Value(T value) { return Result{std::move(value), nullptr}; }

  static Result Error(std::exception_ptr error) {
    return Result{std::nullopt, std::move(error)};
  }

  bool has_value() const { return value.has_value(); }
};

template <typename T>
struct SharedState {
  std::mutex m;
  std::condition_variable cv;

  bool ready = false;
  bool consumed = false;
  bool has_producer = true;

  std::optional<T> value;
  std::exception_ptr error;

  std::function<void(Result<T>)> callback;
};

template <typename T>
class Future;

template <typename T>
class Promise {
 public:
  Promise() : state_(std::make_shared<SharedState<T>>()) {}

  explicit Promise(std::shared_ptr<SharedState<T>> state)
      : state_(std::move(state)) {}

  Future<T> get_future() const;

  void set_value(T value) const {
    set_result(Result<T>::Value(std::move(value)));
  }

  void set_exception(std::exception_ptr error) const {
    set_result(Result<T>::Error(std::move(error)));
  }

  void set_result(Result<T> result) const {
    std::function<void(Result<T>)> callback;
    Result<T> callback_result = result;

    {
      std::lock_guard<std::mutex> lock(state_->m);

      if (state_->ready) {
        throw PromiseAlreadySatisfied();
      }

      if (result.has_value()) {
        state_->value = std::move(*result.value);
      } else {
        state_->error = std::move(result.error);
      }

      state_->ready = true;
      callback = state_->callback;
    }

    state_->cv.notify_all();

    if (callback) {
      callback(std::move(callback_result));
    }
  }

  void close_without_result() const {
    bool should_set_error = false;

    {
      std::lock_guard<std::mutex> lock(state_->m);
      if (!state_->ready) {
        state_->error = std::make_exception_ptr(BrokenPromise());
        state_->ready = true;
        should_set_error = true;
      }
    }

    if (should_set_error) {
      state_->cv.notify_all();
    }
  }

 private:
  std::shared_ptr<SharedState<T>> state_;
};

template <typename T>
class Future {
 public:
  Future() = default;

  explicit Future(std::shared_ptr<SharedState<T>> state)
      : state_(std::move(state)) {}

  bool valid() const { return state_ != nullptr; }

  T get() {
    if (!state_) {
      throw FutureInvalid();
    }

    std::unique_lock<std::mutex> lock(state_->m);
    state_->cv.wait(lock, [this] { return state_->ready; });

    if (state_->consumed) {
      throw FutureInvalid();
    }
    state_->consumed = true;

    if (state_->error) {
      std::rethrow_exception(state_->error);
    }

    return std::move(*state_->value);
  }

  void Subscribe(std::function<void(Result<T>)> callback) const {
    if (!state_) {
      throw FutureInvalid();
    }

    std::optional<Result<T>> ready_result;

    {
      std::lock_guard<std::mutex> lock(state_->m);

      if (!state_->ready) {
        state_->callback = std::move(callback);
        return;
      }

      if (state_->error) {
        ready_result = Result<T>::Error(state_->error);
      } else {
        ready_result = Result<T>::Value(*state_->value);
      }
    }

    callback(std::move(*ready_result));
  }

  template <typename F>
  auto ThenValue(F func) const -> Future<std::invoke_result_t<F, T>> {
    using U = std::invoke_result_t<F, T>;

    Promise<U> next_promise;
    Future<U> next_future = next_promise.get_future();

    Subscribe([next_promise, func = std::move(func)](Result<T> result) mutable {
      try {
        if (!result.has_value()) {
          next_promise.set_exception(result.error);
          return;
        }

        next_promise.set_value(func(std::move(*result.value)));
      } catch (...) {
        next_promise.set_exception(std::current_exception());
      }
    });

    return next_future;
  }

  template <typename F>
  Future<T> ThenError(F handler) const {
    Promise<T> next_promise;
    Future<T> next_future = next_promise.get_future();

    Subscribe(
        [next_promise, handler = std::move(handler)](Result<T> result) mutable {
          try {
            if (result.has_value()) {
              next_promise.set_value(std::move(*result.value));
              return;
            }

            next_promise.set_value(handler(result.error));
          } catch (...) {
            next_promise.set_exception(std::current_exception());
          }
        });

    return next_future;
  }

 private:
  std::shared_ptr<SharedState<T>> state_;
};

template <typename T>
Future<T> Promise<T>::get_future() const {
  return Future<T>(state_);
}

std::string error_text(std::exception_ptr error) {
  try {
    std::rethrow_exception(error);
  } catch (const std::exception& e) {
    return e.what();
  }
}

int main() {
  {
    Promise<int> promise;
    Future<int> future = promise.get_future();

    std::thread producer([promise] { promise.set_value(21); });

    std::cout << "get result: " << future.get() << '\n';
    producer.join();
  }

  {
    Promise<int> promise;
    Future<int> future = promise.get_future();

    Future<std::string> chained =
        future.ThenValue([](int value) { return value * 2; })
            .ThenValue([](int value) {
              return std::string("value = ") + std::to_string(value);
            });

    chained.Subscribe([](Result<std::string> result) {
      std::cout << "ThenValue result: " << *result.value << '\n';
    });

    promise.set_value(21);
  }

  {
    Promise<int> promise;
    Future<int> future = promise.get_future();

    Future<int> recovered =
        future
            .ThenValue([](int) {
              throw std::runtime_error("bad continuation");
              return 0;
            })
            .ThenError([](std::exception_ptr error) {
              std::cout << "ThenError handled: " << error_text(error) << '\n';
              return -1;
            });

    recovered.Subscribe([](Result<int> result) {
      std::cout << "recovered value: " << *result.value << '\n';
    });

    promise.set_value(10);
  }

  {
    Promise<int> promise;
    Future<int> future = promise.get_future();

    promise.close_without_result();

    try {
      future.get();
    } catch (const std::exception& e) {
      std::cout << "broken promise through get(): " << e.what() << '\n';
    }
  }
}
