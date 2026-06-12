#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>

enum class State {
  Empty,
  OnlyResult,
  OnlyCallback,
  Done,
};

template <typename T>
struct SharedState {
  std::mutex m;
  State state = State::Empty;
  std::optional<T> result;
  std::function<void(T)> callback;
};

template <typename T>
class Future {
 public:
  explicit Future(std::shared_ptr<SharedState<T>> state)
      : state_(std::move(state)) {}

  void Subscribe(std::function<void(T)> callback) {
    std::optional<T> ready_result;

    {
      std::lock_guard<std::mutex> lock(state_->m);

      if (state_->state == State::Empty) {
        state_->callback = std::move(callback);
        state_->state = State::OnlyCallback;
        std::cout << "Subscribe first: Empty -> OnlyCallback\n";
        return;
      }

      if (state_->state == State::OnlyResult) {
        ready_result = std::move(state_->result);
        state_->state = State::Done;
        std::cout << "Subscribe second: OnlyResult -> Done\n";
      }
    }

    callback(std::move(*ready_result));
  }

 private:
  std::shared_ptr<SharedState<T>> state_;
};

template <typename T>
class Promise {
 public:
  explicit Promise(std::shared_ptr<SharedState<T>> state)
      : state_(std::move(state)) {}

  Future<T> get_future() { return Future<T>(state_); }

  void set_value(T value) {
    std::function<void(T)> callback;

    {
      std::lock_guard<std::mutex> lock(state_->m);

      if (state_->state == State::Empty) {
        state_->result = std::move(value);
        state_->state = State::OnlyResult;
        std::cout << "set_value first: Empty -> OnlyResult\n";
        return;
      }

      if (state_->state == State::OnlyCallback) {
        callback = std::move(state_->callback);
        state_->state = State::Done;
        std::cout << "set_value second: OnlyCallback -> Done\n";
      }
    }

    callback(std::move(value));
  }

 private:
  std::shared_ptr<SharedState<T>> state_;
};

template <typename T>
std::pair<Future<T>, Promise<T>> make_contract() {
  auto state = std::make_shared<SharedState<T>>();
  Promise<T> promise(state);
  return {promise.get_future(), promise};
}

int main() {
  {
    auto [future, promise] = make_contract<int>();

    future.Subscribe(
        [](int value) { std::cout << "callback got " << value << "\n\n"; });
    promise.set_value(10);
  }

  {
    auto [future, promise] = make_contract<int>();

    promise.set_value(20);
    future.Subscribe(
        [](int value) { std::cout << "callback got " << value << '\n'; });
  }
}
