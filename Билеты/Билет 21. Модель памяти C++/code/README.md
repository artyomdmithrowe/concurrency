# Кодовые заготовки к билету 21

Папки пронумерованы как порядок рассказа на экзамене:

| Папка | Идея |
| --- | --- |
| `01_release_acquire_publication` | `release`/`acquire` публикуют обычные данные через atomic-флаг |
| `02_relaxed_counter` | `relaxed` даёт атомарность счётчика, но не синхронизирует другие данные |
| `03_store_buffering_relaxed` | Store buffering: relaxed допускает результат `r1 == 0 && r2 == 0` |
| `04_commands` | Команды для просмотра asm и запуска примеров |

## Быстрый маршрут

```bash
cd "Билеты/Билет 21. Модель памяти C++/code/01_release_acquire_publication"
./script.sh

cd ../02_relaxed_counter
./script.sh

cd ../03_store_buffering_relaxed
./script.sh
```
