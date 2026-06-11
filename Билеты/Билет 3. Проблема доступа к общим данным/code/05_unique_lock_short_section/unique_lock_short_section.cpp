#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

std::queue<std::function<void()>> tasks;
std::mutex tasks_mutex;

void worker(int id) {
  while (true) {
    std::unique_lock<std::mutex> lock(tasks_mutex);

    if (tasks.empty()) {
      return;
    }

    auto task = std::move(tasks.front());
    tasks.pop();

    lock.unlock();

    task();
    std::cout << "worker #" << id << " finished task\n";
  }
}

int main() {
  for (int i = 0; i < 6; ++i) {
    tasks.push([i] { std::cout << "task #" << i << "\n"; });
  }

  std::vector<std::thread> workers;
  for (int i = 0; i < 3; ++i) {
    workers.emplace_back(worker, i);
  }

  for (auto &thread : workers) {
    thread.join();
  }
}
