# Кодовые заготовки к билету 13

Папки пронумерованы как порядок рассказа на экзамене:

| Папка | Идея |
| --- | --- |
| `01_promise_future` | Базовая пара `std::promise` / `std::future` |
| `02_one_shot_shared_future` | One-shot семантика `future` и многоразовый `shared_future` |
| `03_packaged_task_thread_pool` | `std::packaged_task`: задача в thread pool возвращает `future` |
| `04_subscribe_callback` | Неблокирующий `Subscribe`: callback первым или результат первым |
| `05_then_value_chaining` | Continuation и chaining через учебный `then_value` |
| `06_collect_all_any` | Combining: дождаться все задачи или первую готовую |
| `07_executor_via` | Идея `via`: CPU-работа в pool, ответ обратно в I/O executor |

## Быстрый маршрут

```bash
cd "Билеты/Билет 13. Future и Promise. Асинхронное выполнение задач. Callback hell. Continuations, chaining, combining/code/01_promise_future"
./script.sh

cd ../03_packaged_task_thread_pool
./script.sh
```
