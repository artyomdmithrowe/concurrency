#include <array>
#include <cstdint>
#include <iostream>

struct ExecutionContext {
  void* saved_rsp = nullptr;
};

struct FiberStack {
  std::array<std::uintptr_t, 16> words{};
};

void FiberTrampoline() { std::cout << "trampoline: call user routine\n"; }

void SetupContext(ExecutionContext& context, FiberStack& stack) {
  std::uintptr_t* sp = stack.words.data() + stack.words.size();

  *--sp = reinterpret_cast<std::uintptr_t>(&FiberTrampoline);

  for (int i = 0; i < 6; ++i) {
    *--sp = 0;
  }

  context.saved_rsp = sp;
}

int main() {
  ExecutionContext fiber_context;
  FiberStack fiber_stack;

  SetupContext(fiber_context, fiber_stack);

  std::cout << "prepared saved_rsp = " << fiber_context.saved_rsp << '\n';
}
