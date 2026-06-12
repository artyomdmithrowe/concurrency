#include <atomic>
#include <iostream>
#include <optional>
#include <thread>
#include <vector>

class TreiberStack {
 public:
  ~TreiberStack() {
    while (Node* node = head_.load(std::memory_order_relaxed)) {
      head_.store(node->next, std::memory_order_relaxed);
      delete node;
    }
  }

  void Push(int value) {
    Node* new_node = new Node{value, nullptr};
    Node* old_head = head_.load(std::memory_order_relaxed);

    do {
      new_node->next = old_head;
    } while (!head_.compare_exchange_weak(old_head, new_node,
                                          std::memory_order_release,
                                          std::memory_order_relaxed));
  }

  std::optional<int> PopWithoutDelete() {
    Node* old_head = head_.load(std::memory_order_acquire);

    while (old_head != nullptr) {
      Node* next = old_head->next;

      if (head_.compare_exchange_weak(old_head, next, std::memory_order_acquire,
                                      std::memory_order_relaxed)) {
        return old_head->value;
      }
    }

    return std::nullopt;
  }

 private:
  struct Node {
    int value;
    Node* next;
  };

  std::atomic<Node*> head_{nullptr};
};

int main() {
  TreiberStack stack;
  constexpr int kThreads = 4;
  constexpr int kValuesPerThread = 1000;

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
  while (stack.PopWithoutDelete()) {
    ++popped;
  }

  std::cout << "pushed = " << kThreads * kValuesPerThread << '\n';
  std::cout << "popped = " << popped << '\n';
}
