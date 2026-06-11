#include <iostream>
#include <mutex>

class TreeLikeObject {
public:
    void traverse() {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        std::cout << "traverse\n";
        traverse_impl();
    }

private:
    void traverse_impl() {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        std::cout << "traverse_impl under the same recursive_mutex\n";
    }

    std::recursive_mutex mutex_;
};

class BetterCounter {
public:
    int get() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return get_unlocked();
    }

private:
    int get_unlocked() const {
        return value_;
    }

    mutable std::mutex mutex_;
    int value_ = 42;
};

int main() {
    TreeLikeObject tree;
    tree.traverse();

    BetterCounter counter;
    std::cout << "better counter: " << counter.get() << "\n";
}

