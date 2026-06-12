#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>

template <typename T>
struct SharedState {
  std::mutex m;
  std::optional<T> result;
  std::function<void(const T&)> callback;
  bool done = false;
};

template <typename T>
class MyFuture {
 public:
  explicit MyFuture(std::shared_ptr<SharedState<T>> state)
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

 private:
  std::shared_ptr<SharedState<T>> state_;
};

template <typename T>
class MyPromise {
 public:
  explicit MyPromise(std::shared_ptr<SharedState<T>> state)
      : state_(std::move(state)) {}

  MyFuture<T> get_future() { return MyFuture<T>(state_); }

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
std::pair<MyPromise<T>, MyFuture<T>> MakeContract() {
  auto state = std::make_shared<SharedState<T>>();
  MyPromise<T> promise(state);
  return {promise, promise.get_future()};
}

int main() {
  {
    auto [promise, future] = MakeContract<int>();

    future.Subscribe([](int value) {
      std::cout << "callback first, result later: " << value << '\n';
    });

    std::thread producer([promise]() mutable {
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      promise.set_value(10);
    });

    producer.join();
  }

  {
    auto [promise, future] = MakeContract<int>();

    promise.set_value(20);

    future.Subscribe([](int value) {
      std::cout << "result first, callback later: " << value << '\n';
    });
  }
}
