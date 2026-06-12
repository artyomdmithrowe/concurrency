#include <coroutine>
#include <iostream>
#include <utility>

class Task {
 public:
  struct promise_type {
    Task get_return_object() { return Task{Handle::from_promise(*this)}; }

    std::suspend_always initial_suspend() noexcept { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }

    void return_void() noexcept {}
    void unhandled_exception() { std::terminate(); }
  };

  using Handle = std::coroutine_handle<promise_type>;

  explicit Task(Handle handle) : handle_(handle) {}

  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;

  Task(Task&& other) noexcept : handle_(std::exchange(other.handle_, {})) {}

  ~Task() {
    if (handle_) {
      handle_.destroy();
    }
  }

  void Resume() { handle_.resume(); }
  bool Done() const { return handle_.done(); }

 private:
  Handle handle_;
};

struct ManualEvent {
  std::coroutine_handle<> waiter;
  int value = 0;

  bool await_ready() const noexcept {
    std::cout << "await_ready: result is not ready\n";
    return false;
  }

  void await_suspend(std::coroutine_handle<> handle) noexcept {
    std::cout << "await_suspend: save coroutine handle\n";
    waiter = handle;
  }

  int await_resume() const noexcept {
    std::cout << "await_resume: return result\n";
    return value;
  }

  void SetValue(int new_value) {
    value = new_value;
    waiter.resume();
  }
};

Task Consumer(ManualEvent& event) {
  std::cout << "coroutine: before co_await\n";
  int value = co_await event;
  std::cout << "coroutine: after co_await, value = " << value << '\n';
}

int main() {
  ManualEvent event;
  Task task = Consumer(event);
  task.Resume();

  std::cout << "main: coroutine is suspended\n";

  event.SetValue(7);
  std::cout << "main: done = " << std::boolalpha << task.Done() << '\n';
}
