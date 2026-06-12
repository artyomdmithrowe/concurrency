# Кодовые заготовки к билету 16

Папки пронумерованы как порядок рассказа на экзамене:

| Папка | Идея |
| --- | --- |
| `01_minimal_task_co_return` | Минимальный `Task<int>` и `co_return` |
| `02_manual_awaiter` | Три метода awaiter: `await_ready`, `await_suspend`, `await_resume` |
| `03_task_chaining_continuation` | `co_await` одной задачи из другой и continuation в `final_suspend` |

## Быстрый маршрут

```bash
cd "Билеты/Билет 16. Stackful и stackless корутины. C++ coroutines, `co_await`, `co_return`/code/01_minimal_task_co_return"
./script.sh

cd ../02_manual_awaiter
./script.sh

cd ../03_task_chaining_continuation
./script.sh
```
