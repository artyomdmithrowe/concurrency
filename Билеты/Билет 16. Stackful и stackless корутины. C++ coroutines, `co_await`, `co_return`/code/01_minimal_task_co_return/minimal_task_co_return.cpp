#include <coroutine>
#include <iostream>
#include <utility>

class IntTask {
 public:
  struct promise_type {
    int value = 0;

    IntTask get_return_object() { return IntTask{Handle::from_promise(*this)}; }

    std::suspend_always initial_suspend() noexcept { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }

    void return_value(int result) noexcept { value = result; }

    void unhandled_exception() { std::terminate(); }
  };

  using Handle = std::coroutine_handle<promise_type>;

  explicit IntTask(Handle handle) : handle_(handle) {}

  IntTask(const IntTask&) = delete;
  IntTask& operator=(const IntTask&) = delete;

  IntTask(IntTask&& other) noexcept
      : handle_(std::exchange(other.handle_, {})) {}

  ~IntTask() {
    if (handle_) {
      handle_.destroy();
    }
  }

  int Run() {
    handle_.resume();
    return handle_.promise().value;
  }

 private:
  Handle handle_;
};

IntTask Answer() {
  std::cout << "coroutine body starts\n";
  co_return 42;
}

int main() {
  IntTask task = Answer();

  std::cout << "Answer() returned IntTask\n";
  int result = task.Run();
  std::cout << "result = " << result << '\n';
}
