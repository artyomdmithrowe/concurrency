# Кодовые заготовки к билету 7

Папки пронумерованы как порядок рассказа на экзамене:

| Папка | Идея |
| --- | --- |
| `01_wait_wake` | Минимальный `FUTEX_WAIT` / `FUTEX_WAKE` |
| `02_expected_value_eagain` | `FUTEX_WAIT` не засыпает, если значение уже не равно `expected` |
| `03_two_state_futex_mutex` | Учебный mutex на двух состояниях: fast path в userspace, slow path через futex |
| `04_wait_loop_wake_all` | После пробуждения нужно снова проверять состояние |
| `05_adaptive_mutex` | Сначала короткий spin, потом `futex_wait` |
| `06_commands` | Команды, которыми можно увидеть futex-системные вызовы |

## Быстрый маршрут

```bash
cd "Билеты/Билет 7. Системный вызов futex/code/01_wait_wake"
./script.sh

cd ../03_two_state_futex_mutex
./script.sh
```
