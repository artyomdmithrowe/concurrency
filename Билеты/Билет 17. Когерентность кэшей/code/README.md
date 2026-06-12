# Кодовые заготовки к билету 17

Папки пронумерованы как порядок рассказа на экзамене:

| Папка | Идея |
| --- | --- |
| `01_cache_line_layout` | Размер cache line и адреса соседних/разнесённых счётчиков |
| `02_false_sharing_benchmark` | False sharing: разные счётчики, одна cache line |
| `03_global_vs_per_thread_counter` | True sharing на глобальном счётчике и per-thread counters |
| `04_commands` | Команды для просмотра cache line/cache-информации |

## Быстрый маршрут

```bash
cd "Билеты/Билет 17. Когерентность кэшей/code/01_cache_line_layout"
./script.sh

cd ../02_false_sharing_benchmark
./script.sh

cd ../03_global_vs_per_thread_counter
./script.sh
```
