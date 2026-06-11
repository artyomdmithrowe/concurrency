#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex m1;
std::mutex m2;

void safe_transfer_1() {
  std::scoped_lock lock(m1, m2);
  std::cout << "safe_transfer_1 locked both mutexes\n";
}

void safe_transfer_2() {
  std::scoped_lock lock(m2, m1);
  std::cout << "safe_transfer_2 locked both mutexes\n";
}

void safe_transfer_with_std_lock_1() {
  std::unique_lock lock1(m1, std::defer_lock);
  std::unique_lock lock2(m2, std::defer_lock);

  std::lock(lock1, lock2);

  std::cout << "safe_transfer_with_std_lock_1 locked both mutexes\n";
}

void safe_transfer_with_std_lock_2() {
  std::unique_lock lock2(m2, std::defer_lock);
  std::unique_lock lock1(m1, std::defer_lock);

  std::lock(lock2, lock1);

  std::cout << "safe_transfer_with_std_lock_2 locked both mutexes\n";
}

void unsafe_transfer_1() {
  std::lock_guard lock1(m1);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::lock_guard lock2(m2);

  std::cout << "unsafe_transfer_1 finish work\n";
}

void unsafe_transfer_2() {
  std::lock_guard lock2(m2);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::lock_guard lock1(m1);

  std::cout << "unsafe_transfer_2 finish work\n";
}

int main() {
  std::cout << std::unitbuf;

  std::thread t1(safe_transfer_1);
  std::thread t2(safe_transfer_2);

  t1.join();
  t2.join();

  std::cout << "std::scoped_lock avoided deadlock even with different argument "
               "order.\n";

  std::thread t3(safe_transfer_with_std_lock_1);
  std::thread t4(safe_transfer_with_std_lock_2);

  t3.join();
  t4.join();

  std::cout << "std::lock + defer_lock avoided deadlock too.\n";

  std::thread t5(unsafe_transfer_1);
  std::thread t6(unsafe_transfer_2);

  t5.join();
  t6.join();
}
