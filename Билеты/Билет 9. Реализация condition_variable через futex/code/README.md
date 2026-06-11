# Кодовые заготовки к билету 9

Папки пронумерованы как порядок рассказа на экзамене:

| Папка | Идея |
| --- | --- |
| `01_generation_counter` | `seq` как счётчик поколений: `notify` меняет значение, `wait` ждёт старое |
| `02_queue_notify_one` | Простая `condition_variable` через futex и внешнее состояние под mutex |
| `03_atomic_wait` | Похожая идея через `std::atomic::wait/notify_one` |
| `04_commands` | Команды для просмотра futex-системных вызовов |

## Быстрый маршрут

```bash
cd "Билеты/Билет 9. Реализация condition_variable через futex/code/01_generation_counter"
./script.sh

cd ../02_queue_notify_one
./script.sh
```
