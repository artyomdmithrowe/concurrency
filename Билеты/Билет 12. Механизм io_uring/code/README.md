# Кодовые заготовки к билету 12

Папки пронумерованы как порядок рассказа на экзамене:

| Папка | Идея |
| --- | --- |
| `01_ring_basics` | Минимальный `io_uring`: SQE, submit, CQE |
| `02_read_once` | Одна операция `read`: результат приходит в `cqe->res` |
| `03_user_data` | `user_data` связывает completion с заявкой |
| `04_io_uring_echo_server` | Однопоточный echo-сервер: `accept -> recv -> send -> recv` |
| `05_multithread_io_uring_server` | Несколько `io_uring` loop-ов через `SO_REUSEPORT` |
| `06_commands` | Команды для `nc`, `ss`, `strace`, `man` |

## Быстрый маршрут

```bash
cd "Билеты/Билет 12. Механизм io_uring/code/01_ring_basics"
./script.sh

cd ../04_io_uring_echo_server
./script.sh
```
