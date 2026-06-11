# Кодовые заготовки к билету 10

Папки пронумерованы как порядок рассказа на экзамене:

| Папка | Идея |
| --- | --- |
| `01_nonblocking_read` | `read` на неблокирующем fd: данные, `EAGAIN`, EOF |
| `02_nonblocking_write_buffer` | Неблокирующий `write`: можно записать не всё, остаток хранится в `output_buffer` |
| `03_busy_loop_echo_server` | Busy-loop echo server: `accept/read/write` до `EAGAIN` |
| `04_commands` | Команды для `nc`, `ss`, `strace`, `top` |

## Быстрый маршрут

```bash
cd "Билеты/Билет 10. Неблокирующие `read` и `write`. Busy loop polling. Проблемы такой реализации/code/01_nonblocking_read"
./script.sh

cd ../03_busy_loop_echo_server
./script.sh
```
