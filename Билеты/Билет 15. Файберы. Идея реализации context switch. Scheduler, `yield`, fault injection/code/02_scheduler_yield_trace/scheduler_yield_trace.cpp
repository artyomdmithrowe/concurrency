#include <deque>
#include <iostream>
#include <string>

struct FiberTrace {
  std::string name;
  int step = 0;
};

void PrintQueue(const std::deque<FiberTrace>& queue) {
  std::cout << "run_queue = [";
  for (std::size_t i = 0; i < queue.size(); ++i) {
    if (i != 0) {
      std::cout << ", ";
    }
    std::cout << queue[i].name;
  }
  std::cout << "]\n";
}

bool RunOneStep(FiberTrace& fiber) {
  if (fiber.step == 0) {
    std::cout << fiber.name << " prints " << fiber.name << "1\n";
    std::cout << fiber.name << " calls yield: Running -> Runnable\n";
    ++fiber.step;
    return false;
  }

  std::cout << fiber.name << " prints " << fiber.name << "2\n";
  std::cout << fiber.name << " returns: Running -> Dead\n";
  ++fiber.step;
  return true;
}

int main() {
  std::deque<FiberTrace> run_queue;
  run_queue.push_back(FiberTrace{"A"});
  run_queue.push_back(FiberTrace{"B"});

  PrintQueue(run_queue);

  while (!run_queue.empty()) {
    FiberTrace current = run_queue.front();
    run_queue.pop_front();

    std::cout << "\nscheduler picks " << current.name << '\n';
    bool dead = RunOneStep(current);

    if (!dead) {
      run_queue.push_back(current);
    }

    PrintQueue(run_queue);
  }
}
