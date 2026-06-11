#include <chrono>
#include <iostream>
#include <thread>

void work(int id) {
  std::cout << "thread #" << id << " started\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::cout << "thread #" << id << " finished\n";
}

int main() {
  {
    std::thread t(work, 1);
    t.detach();
  }

  {
    std::thread t(work, 2);
    t.join();
  }

  {
    std::thread t(work, 3);
    t.join();
  }

  {
    std::thread t(work, 4);
    t.detach();
  }

  int client_fd = 42;

  std::thread good([client_fd] {
    std::cout << "fd copied into thread: " << client_fd << "\n";
  });
  good.join();

  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}
