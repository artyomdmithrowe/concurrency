#include <atomic>
#include <iostream>
#include <optional>
#include <thread>
#include <vector>

class MichaelScottQueue {
 public:
  MichaelScottQueue() {
    Node* dummy = new Node{0, nullptr};
    head_.store(dummy, std::memory_order_relaxed);
    tail_.store(dummy, std::memory_order_relaxed);
  }

  void Enqueue(int value) {
    Node* new_node = new Node{value, nullptr};

    while (true) {
      Node* tail = tail_.load(std::memory_order_acquire);
      Node* next = tail->next.load(std::memory_order_acquire);

      if (tail != tail_.load(std::memory_order_acquire)) {
        continue;
      }

      if (next == nullptr) {
        if (tail->next.compare_exchange_weak(next, new_node,
                                             std::memory_order_release,
                                             std::memory_order_relaxed)) {
          tail_.compare_exchange_weak(tail, new_node, std::memory_order_release,
                                      std::memory_order_relaxed);
          return;
        }
      } else {
        tail_.compare_exchange_weak(tail, next, std::memory_order_release,
                                    std::memory_order_relaxed);
      }
    }
  }

  std::optional<int> DequeueWithoutDelete() {
    while (true) {
      Node* head = head_.load(std::memory_order_acquire);
      Node* tail = tail_.load(std::memory_order_acquire);
      Node* next = head->next.load(std::memory_order_acquire);

      if (head != head_.load(std::memory_order_acquire)) {
        continue;
      }

      if (head == tail) {
        if (next == nullptr) {
          return std::nullopt;
        }

        tail_.compare_exchange_weak(tail, next, std::memory_order_release,
                                    std::memory_order_relaxed);
        continue;
      }

      int value = next->value;
      if (head_.compare_exchange_weak(head, next, std::memory_order_acquire,
                                      std::memory_order_relaxed)) {
        return value;
      }
    }
  }

 private:
  struct Node {
    int value;
    std::atomic<Node*> next;
  };

  std::atomic<Node*> head_{nullptr};
  std::atomic<Node*> tail_{nullptr};
};

int main() {
  MichaelScottQueue queue;

  constexpr int kThreads = 4;
  constexpr int kValuesPerThread = 1000;

  std::vector<std::thread> producers;
  for (int id = 0; id < kThreads; ++id) {
    producers.emplace_back([&, id] {
      for (int i = 0; i < kValuesPerThread; ++i) {
        queue.Enqueue(id * kValuesPerThread + i);
      }
    });
  }

  for (std::thread& producer : producers) {
    producer.join();
  }

  int dequeued = 0;
  while (queue.DequeueWithoutDelete()) {
    ++dequeued;
  }

  std::cout << "enqueued = " << kThreads * kValuesPerThread << '\n';
  std::cout << "dequeued = " << dequeued << '\n';
}
