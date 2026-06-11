# Кодовые заготовки к билету 5

Папки пронумерованы как порядок рассказа на экзамене:

| Папка | Идея |
| --- | --- |
| `01_blocking_queue` | Очередь как коммуникация между producer и consumer |
| `02_blocking_queue_close` | `close`, `closed_`, `notify_all` и выход workers |
| `03_thread_pool_basic` | Минимальный thread pool над блокирующей очередью |
| `04_exception_safety` | Worker ловит исключения задач и не умирает |
| `05_wait_group` | Отдельный инструмент для ожидания группы задач |
| `06_future_result` | Возврат результата через `packaged_task` и `future` |

## Быстрый маршрут

```bash
cd "Билеты/Билет 5. Thread Pool with Blocking Queue/code/03_thread_pool_basic"
./script.sh
```
