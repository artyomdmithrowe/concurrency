#include <coroutine>
#include <exception>
#include <iostream>
#include <optional>
#include <utility>

class IntTask {
 public:
  struct promise_type;
  using Handle = std::coroutine_handle<promise_type>;

  struct FinalAwaiter {
    bool await_ready() noexcept { return false; }

    std::coroutine_handle<> await_suspend(Handle handle) noexcept {
      return handle.promise().continuation;
    }

    void await_resume() noexcept {}
  };

  struct promise_type {
    std::optional<int> result;
    std::exception_ptr error;
    std::coroutine_handle<> continuation = std::noop_coroutine();

    IntTask get_return_object() { return IntTask{Handle::from_promise(*this)}; }

    std::suspend_always initial_suspend() noexcept { return {}; }
    FinalAwaiter final_suspend() noexcept { return {}; }

    void return_value(int value) noexcept { result = value; }
    void unhandled_exception() noexcept { error = std::current_exception(); }
  };

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

  struct Awaiter {
    Handle child;

    bool await_ready() noexcept { return false; }

    std::coroutine_handle<> await_suspend(
        std::coroutine_handle<> parent) noexcept {
      child.promise().continuation = parent;
      return child;
    }

    int await_resume() {
      if (child.promise().error) {
        std::exception_ptr error = child.promise().error;
        child.destroy();
        std::rethrow_exception(error);
      }

      int result = *child.promise().result;
      child.destroy();
      return result;
    }
  };

  Awaiter operator co_await() && {
    Handle child = std::exchange(handle_, {});
    return Awaiter{child};
  }

  int Run() && {
    Handle handle = std::exchange(handle_, {});
    handle.resume();

    if (handle.promise().error) {
      std::rethrow_exception(handle.promise().error);
    }

    int result = *handle.promise().result;
    handle.destroy();
    return result;
  }

 private:
  Handle handle_;
};

IntTask Child() {
  std::cout << "child: co_return 10\n";
  co_return 10;
}

IntTask Parent() {
  std::cout << "parent: before co_await child\n";
  int value = co_await Child();
  std::cout << "parent: child result = " << value << '\n';
  co_return value + 1;
}

int main() {
  IntTask task = Parent();
  int result = std::move(task).Run();
  std::cout << "main: result = " << result << '\n';
}
