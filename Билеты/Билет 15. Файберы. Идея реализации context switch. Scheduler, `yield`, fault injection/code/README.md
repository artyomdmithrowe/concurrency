# Кодовые заготовки к билету 15

Папки пронумерованы как порядок рассказа на экзамене:

| Папка | Идея |
| --- | --- |
| `01_context_switch_sketch` | Что хранит execution context и почему нужен trampoline |
| `02_scheduler_yield_trace` | Trace-модель FIFO scheduler и добровольный `yield` |
| `03_fiber_runtime_pseudocode` | Скелет `SwitchContext`, `Go`, `Run`, `Yield`, `Suspend`, `Resume` |

## Быстрый маршрут

```bash
cd "Билеты/Билет 15. Файберы. Идея реализации context switch. Scheduler, `yield`, fault injection/code/02_scheduler_yield_trace"
./script.sh
```

`03_fiber_runtime_pseudocode/fiber_runtime_pseudocode.cpp` не запускается, это шпаргалка для объяснения реализации.
