# Кодовые заготовки к билету 14

Папки пронумерованы как порядок рассказа на экзамене:

| Папка | Идея |
| --- | --- |
| `01_shared_state_value` | Минимальная реализация `Future`/`Promise`: shared state, `set_value`, `get` |
| `02_exception_broken_promise` | `set_exception` и broken promise в деструкторе |
| `03_one_shot_move_only` | Почему обычный `future` one-shot и move-only |
| `04_subscribe_state_machine` | `Subscribe`: callback первым или result первым |
| `05_then_value_then_error` | `ThenValue` и `ThenError` как обёртки над `Subscribe` |
| `06_complete_future` | Сводный файл: `Future`, `Promise`, `Subscribe`, `ThenValue`, `ThenError` |

## Быстрый маршрут

```bash
cd "Билеты/Билет 14. Реализация `std-future` и `std-promise`/code/01_shared_state_value"
./script.sh

cd ../02_exception_broken_promise
./script.sh
```
