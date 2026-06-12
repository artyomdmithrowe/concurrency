#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <type_traits>

template <typename T>
struct SharedState {
  std::mutex m;
  std::optional<T> result;
  std::function<void(const T&)> callback;
  bool done = false;
};

template <typename T>
class Future;

template <typename T>
class Promise {
 public:
  explicit Promise(std::shared_ptr<SharedState<T>> state)
      : state_(std::move(state)) {}

  Future<T> get_future();

  void set_value(T value) {
    std::function<void(const T&)> callback;
    std::optional<T> ready_result;

    {
      std::lock_guard<std::mutex> lock(state_->m);
      state_->result = std::move(value);

      if (state_->callback && !state_->done) {
        callback = std::move(state_->callback);
        ready_result = state_->result;
        state_->done = true;
      }
    }

    if (callback) {
      callback(*ready_result);
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

  void Subscribe(std::function<void(const T&)> callback) {
    std::optional<T> ready_result;

    {
      std::lock_guard<std::mutex> lock(state_->m);

      if (state_->result.has_value() && !state_->done) {
        ready_result = state_->result;
        state_->done = true;
      } else {
        state_->callback = std::move(callback);
        return;
      }
    }

    callback(*ready_result);
  }

  template <typename F>
  auto then_value(F f) -> Future<std::invoke_result_t<F, T>> {
    using U = std::invoke_result_t<F, T>;

    auto next_state = std::make_shared<SharedState<U>>();
    Promise<U> next_promise(next_state);
    Future<U> next_future(next_state);

    Subscribe([next_promise, f = std::move(f)](const T& value) mutable {
      next_promise.set_value(f(value));
    });

    return next_future;
  }

 private:
  std::shared_ptr<SharedState<T>> state_;
};

template <typename T>
Future<T> Promise<T>::get_future() {
  return Future<T>(state_);
}

template <typename T>
std::pair<Promise<T>, Future<T>> MakeContract() {
  auto state = std::make_shared<SharedState<T>>();
  Promise<T> promise(state);
  return {promise, promise.get_future()};
}

int main() {
  auto [promise, future] = MakeContract<int>();

  auto result = future
                    .then_value([](int x) {
                      std::cout << "step 1: " << x << " * 2\n";
                      return x * 2;
                    })
                    .then_value([](int x) {
                      std::cout << "step 2: convert " << x << " to string\n";
                      return std::string("answer = ") + std::to_string(x);
                    });

  result.Subscribe([](const std::string& text) {
    std::cout << "final continuation: " << text << '\n';
  });

  promise.set_value(21);
}
