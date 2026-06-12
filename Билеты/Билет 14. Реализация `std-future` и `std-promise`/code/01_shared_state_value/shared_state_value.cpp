#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>

template <typename T>
struct SharedState {
  std::mutex m;
  std::condition_variable cv;
  bool ready = false;
  std::optional<T> value;
};

template <typename T>
class Future {
 public:
  explicit Future(std::shared_ptr<SharedState<T>> state)
      : state_(std::move(state)) {}

  T get() {
    std::unique_lock<std::mutex> lock(state_->m);
    state_->cv.wait(lock, [this] { return state_->ready; });
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

  void set_value(T value) {
    {
      std::lock_guard<std::mutex> lock(state_->m);
      state_->value = std::move(value);
      state_->ready = true;
    }

    state_->cv.notify_all();
  }

 private:
  std::shared_ptr<SharedState<T>> state_;
};

template <typename T>
std::pair<Future<T>, Promise<T>> make_contract() {
  auto state = std::make_shared<SharedState<T>>();
  return {Future<T>(state), Promise<T>(state)};
}

int main() {
  auto [future, promise] = make_contract<int>();

  std::thread producer(
      [promise = std::move(promise)]() mutable { promise.set_value(42); });

  std::cout << "future.get(): " << future.get() << '\n';

  producer.join();
}
