#include <cstdint>
#include <deque>
#include <string>

enum class State {
  Runnable,
  Running,
  Suspended,
  Dead,
};

struct ExecutionContext {
  void* rsp = nullptr;
};

struct Fiber {
  ExecutionContext context;
  State state = State::Runnable;
  Stack stack;
  Function routine;
};

extern "C" void SwitchContext(void** from_rsp, void* to_rsp);

void FiberTrampoline();

void SetupContext(ExecutionContext& context, Stack& stack) {
  std::uintptr_t* sp = stack.Top();

  *--sp = reinterpret_cast<std::uintptr_t>(&FiberTrampoline);

  for (int i = 0; i < 6; ++i) {
    *--sp = 0;
  }

  context.rsp = sp;
}

class Scheduler {
 public:
  static Scheduler* Current();

  Fiber* CurrentFiber() { return current_; }

  void Go(Function routine) {
    Fiber* fiber = new Fiber;

    fiber->stack = AllocateStack();
    fiber->routine = routine;
    fiber->state = State::Runnable;

    SetupContext(fiber->context, fiber->stack);
    run_queue_.push_back(fiber);
  }

  void Run() {
    while (!run_queue_.empty() || poller_.HasWaiters()) {
      if (run_queue_.empty()) {
        PollEvents();
        continue;
      }

      Fiber* fiber = run_queue_.front();
      run_queue_.pop_front();

      current_ = fiber;
      current_->state = State::Running;

      SwitchContext(&context_.rsp, current_->context.rsp);

      if (current_->state == State::Dead) {
        delete current_;
      }

      current_ = nullptr;
    }
  }

  void Yield() {
    Fiber* self = current_;

    self->state = State::Runnable;
    run_queue_.push_back(self);

    SwitchContext(&self->context.rsp, context_.rsp);
  }

  void Suspend() {
    Fiber* self = current_;

    self->state = State::Suspended;

    SwitchContext(&self->context.rsp, context_.rsp);
  }

  void Resume(Fiber* fiber) {
    if (fiber->state != State::Suspended) {
      return;
    }

    fiber->state = State::Runnable;
    run_queue_.push_back(fiber);
  }

  void ExitCurrentFiber() {
    current_->state = State::Dead;

    SwitchContext(&current_->context.rsp, context_.rsp);
  }

  void WaitRead(int fd) {
    Fiber* self = current_;

    poller_.AddRead(fd, self);
    Suspend();
  }

  void PollEvents() {
    for (Event event : poller_.Wait()) {
      if (event.readable) {
        poller_.Remove(event.fd);
        Resume(event.fiber);
      }
    }
  }

 private:
  std::deque<Fiber*> run_queue_;
  Fiber* current_ = nullptr;
  ExecutionContext context_;
  Poller poller_;
};

void FiberTrampoline() {
  Scheduler* scheduler = Scheduler::Current();
  Fiber* fiber = scheduler->CurrentFiber();

  fiber->routine();

  scheduler->ExitCurrentFiber();
}

std::string Read(int fd) {
  char buffer[4096];

  while (true) {
    ssize_t n = read(fd, buffer, sizeof(buffer));

    if (n > 0) {
      return std::string(buffer, buffer + n);
    }

    if (n == 0) {
      return {};
    }

    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      Scheduler::Current()->WaitRead(fd);
      continue;
    }

    throw SystemError(errno);
  }
}

void Example() {
  Scheduler scheduler;

  scheduler.Go([] {
    Print("A1");
    Scheduler::Current()->Yield();
    Print("A2");
  });

  scheduler.Go([] {
    Print("B1");
    Scheduler::Current()->Yield();
    Print("B2");
  });

  scheduler.Run();
}

void Session(int fd) {
  SetNonBlocking(fd);

  while (true) {
    std::string request = Read(fd);

    if (request.empty()) {
      break;
    }

    Handle(request);
  }
}
