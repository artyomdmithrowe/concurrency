#include <atomic>
#include <cstddef>
#include <iostream>
#include <new>

struct Counter {
  std::atomic<long long> value{0};
};

struct alignas(std::hardware_destructive_interference_size) PaddedCounter {
  std::atomic<long long> value{0};
};

template <class T>
void PrintArrayLayout(const char* name, T* counters, std::size_t size) {
  std::cout << name << '\n';
  std::cout << "  sizeof = " << sizeof(T) << '\n';
  std::cout << "  alignof = " << alignof(T) << '\n';

  for (std::size_t i = 0; i < size; ++i) {
    std::cout << "  &counters[" << i << "] = " << &counters[i] << '\n';
  }

  std::cout << '\n';
}

int main() {
  std::cout << "hardware_destructive_interference_size = "
            << std::hardware_destructive_interference_size << '\n';
  std::cout << "hardware_constructive_interference_size = "
            << std::hardware_constructive_interference_size << "\n\n";

  Counter adjacent[4];
  PaddedCounter padded[4];

  PrintArrayLayout("Counter adjacent[4]", adjacent, 4);
  PrintArrayLayout("PaddedCounter padded[4]", padded, 4);
}
