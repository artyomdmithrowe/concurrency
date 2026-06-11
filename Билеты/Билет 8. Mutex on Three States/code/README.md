# Кодовые заготовки к билету 8

Папки пронумерованы как порядок рассказа на экзамене:

| Папка | Идея |
| --- | --- |
| `00_oneshot_use_after_free` | Искусственно расширенное окно между `store(0)` и `futex_wake`: ASan показывает use-after-free |
| `01_two_state_extra_wake` | Двухсостояниевый futex-mutex всегда вызывает `futex_wake` в `unlock` |
| `02_separate_waiting_race` | Почему отдельный `waiting` рядом с `state` не решает проблему |
| `03_three_state_mutex` | Mutex на трёх состояниях: `0`, `1`, `2` |
| `04_uncontended_compare` | На неконкурентном пути `Mutex3` не делает лишний `wake` |
| `05_commands` | Команды для просмотра futex-системных вызовов |

## Быстрый маршрут

```bash
cd "Билеты/Билет 8. Mutex on Three States/code/00_oneshot_use_after_free"
./script.sh

cd ../01_two_state_extra_wake
./script.sh

cd ../03_three_state_mutex
./script.sh
```
