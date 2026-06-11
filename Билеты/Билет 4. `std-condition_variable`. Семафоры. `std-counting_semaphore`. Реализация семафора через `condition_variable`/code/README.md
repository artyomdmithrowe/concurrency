# Кодовые заготовки к билету 4

Папки пронумерованы как порядок рассказа на экзамене:

| Папка | Идея |
| --- | --- |
| `01_busy_wait_bad` | Плохой старт: busy wait и data race |
| `02_mutex_sleep_bad` | Мьютекс убрал data race, но появился sleep/polling |
| `03_condition_variable_semaphore` | Корректный семафор через `condition_variable` |
| `04_if_vs_while` | Почему `wait` должен быть в `while` или с predicate |
| `05_counting_semaphore` | Готовый `std::counting_semaphore` из C++20 |
| `06_semaphore_guard` | RAII-обёртка, чтобы `release` не забывался |
| `07_blocking_queue` | Очередь: `push` + `notify_one`, `pop` + `wait` |
| `08_blocking_queue_stop` | Очередь со `stop` и `notify_all` |
| `09_wait_timeout` | Таймауты через `wait_for` |

## Быстрый маршрут

```bash
cd "Билеты/Билет 4. `std-condition_variable`. Семафоры. `std-counting_semaphore`. Реализация семафора через `condition_variable`/code/03_condition_variable_semaphore"
./script.sh
```
