# Кодовые заготовки к билету 11

Папки пронумерованы как порядок рассказа на экзамене:

| Папка | Идея |
| --- | --- |
| `01_select_ready` | `select`: собираем `fd_set`, ждём готовности, проверяем `FD_ISSET` |
| `02_poll_ready` | `poll`: массив `pollfd`, события в `revents` |
| `03_epoll_echo_server` | Однопоточный epoll echo server: `epoll_create1`, `epoll_ctl`, `epoll_wait` |
| `04_edge_triggered_read_until_eagain` | Edge-triggered идея: после события читаем до `EAGAIN` |
| `05_edge_triggered_missed_event` | Что будет, если в `EPOLLET` не дочитать данные до `EAGAIN` |
| `06_multithread_epoll_server` | Многопоточный epoll-сервер: несколько event loop-ов через `SO_REUSEPORT` |
| `07_commands` | Команды для `nc`, `ss`, `strace`, `top` |

## Быстрый маршрут

```bash
cd "Билеты/Билет 11. select, poll, epoll/code/01_select_ready"
./script.sh

cd ../03_epoll_echo_server
./script.sh
```
