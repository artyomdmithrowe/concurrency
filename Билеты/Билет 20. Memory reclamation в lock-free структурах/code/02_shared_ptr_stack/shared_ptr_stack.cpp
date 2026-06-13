#include <atomic>
#include <iostream>
#include <memory>
#include <optional>
#include <thread>
#include <vector>

class SharedPtrStack {
 public:
  void Push(int value) {
    auto new_node = std::make_shared<Node>();
    new_node->value = value;

    std::shared_ptr<Node> old_head = head_.load();

    do {
      new_node->next = old_head;
    } while (!head_.compare_exchange_weak(old_head, new_node));
  }

  std::optional<int> Pop() {
    std::shared_ptr<Node> old_head = head_.load();

    while (old_head) {
      std::shared_ptr<Node> next = old_head->next;

      if (head_.compare_exchange_weak(old_head, next)) {
        return old_head->value;
      }
    }

    return std::nullopt;
  }

  bool IsLockFree() const { return head_.is_lock_free(); }

 private:
  struct Node {
    int value = 0;
    std::shared_ptr<Node> next;
  };

  std::atomic<std::shared_ptr<Node>> head_;
};

int main() {
  SharedPtrStack stack;
  constexpr int kThreads = 4;
  constexpr int kValuesPerThread = 1000;

  std::cout << "atomic shared_ptr head is_lock_free = " << std::boolalpha
            << stack.IsLockFree() << '\n';

  std::vector<std::thread> pushers;
  for (int id = 0; id < kThreads; ++id) {
    pushers.emplace_back([&, id] {
      for (int i = 0; i < kValuesPerThread; ++i) {
        stack.Push(id * kValuesPerThread + i);
      }
    });
  }

  for (std::thread& thread : pushers) {
    thread.join();
  }

  int popped = 0;
  while (stack.Pop()) {
    ++popped;
  }

  std::cout << "pushed = " << kThreads * kValuesPerThread << '\n';
  std::cout << "popped = " << popped << '\n';
}
