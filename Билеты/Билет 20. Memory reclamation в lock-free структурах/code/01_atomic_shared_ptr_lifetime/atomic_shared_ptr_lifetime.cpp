#include <atomic>
#include <iostream>
#include <memory>
#include <string>

struct Node {
  explicit Node(std::string name) : name(std::move(name)) {
    std::cout << "construct " << this->name << '\n';
  }

  ~Node() { std::cout << "destroy " << name << '\n'; }

  std::string name;
};

int main() {
  std::atomic<std::shared_ptr<Node>> head;

  head.store(std::make_shared<Node>("A"));

  std::cout << "atomic_shared_ptr is_lock_free = " << std::boolalpha
            << head.is_lock_free() << "\n\n";

  std::shared_ptr<Node> local = head.load();
  std::cout << "reader loaded " << local->name << '\n';
  std::cout << "local use_count = " << local.use_count() << "\n\n";

  head.store(nullptr);
  std::cout << "local still points to " << local->name << '\n';
  std::cout << "local use_count = " << local.use_count() << "\n\n";

  local.reset();
}
