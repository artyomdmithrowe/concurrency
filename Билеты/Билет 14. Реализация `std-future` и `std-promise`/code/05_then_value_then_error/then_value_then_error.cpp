#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>

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
  std::optional<Result<T>> result;
  std::function<void(Result<T>)> callback;
};

template <typename T>
class Future;

template <typename T>
class Promise {
 public:
  Promise() : state_(std::make_shared<SharedState<T>>()) {}

  Future<T> get_future() const;

  void set_value(T value) const {
    set_result(Result<T>::Value(std::move(value)));
  }

  void set_error(std::exception_ptr error) const {
    set_result(Result<T>::Error(std::move(error)));
  }

  void set_result(Result<T> result) const {
    std::function<void(Result<T>)> callback;

    {
      std::lock_guard<std::mutex> lock(state_->m);
      state_->result = std::move(result);
      callback = std::move(state_->callback);
    }

    if (callback) {
      callback(*state_->result);
    }
  }

 private:
  std::shared_ptr<SharedState<T>> state_;
};

template <typename T>
class Future {
 public:
  explicit Future(std::shared_ptr<SharedState<T>> state)
      : state_(std::move(state)) {}

  void Subscribe(std::function<void(Result<T>)> callback) const {
    std::optional<Result<T>> ready_result;

    {
      std::lock_guard<std::mutex> lock(state_->m);

      if (state_->result.has_value()) {
        ready_result = state_->result;
      } else {
        state_->callback = std::move(callback);
        return;
      }
    }

    callback(*ready_result);
  }

  template <typename F>
  auto ThenValue(F func) const -> Future<std::invoke_result_t<F, T>> {
    using U = std::invoke_result_t<F, T>;

    Promise<U> next_promise;
    Future<U> next_future = next_promise.get_future();

    Subscribe([next_promise, func = std::move(func)](Result<T> result) mutable {
      try {
        if (!result.has_value()) {
          next_promise.set_error(result.error);
          return;
        }

        next_promise.set_value(func(std::move(*result.value)));
      } catch (...) {
        next_promise.set_error(std::current_exception());
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
            next_promise.set_error(std::current_exception());
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

    Future<std::string> chained =
        future.ThenValue([](int value) { return value * 2; })
            .ThenValue([](int value) {
              return std::string("value = ") + std::to_string(value);
            });

    chained.Subscribe(
        [](Result<std::string> result) { std::cout << *result.value << '\n'; });

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
      std::cout << "recovered value = " << *result.value << '\n';
    });

    promise.set_value(10);
  }
}
