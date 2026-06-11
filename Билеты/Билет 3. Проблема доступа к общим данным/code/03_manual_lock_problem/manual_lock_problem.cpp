#include <iostream>
#include <mutex>
#include <stdexcept>

std::mutex mutex;

void bad_manual_lock() {
  mutex.lock();
  throw std::runtime_error("oops before unlock");
  mutex.unlock();
}

void good_raii_lock() {
  std::lock_guard<std::mutex> lock(mutex);
  throw std::runtime_error("oops, but lock_guard will unlock");
}

int main() {
  //   try {
  //     bad_manual_lock();
  //   } catch (const std::exception &error) {
  //     std::cout << "Manual lock version caught: " << error.what() << "\n";
  //   }

  try {
    good_raii_lock();
  } catch (const std::exception &error) {
    std::cout << "RAII version caught: " << error.what() << "\n";
  }
}
