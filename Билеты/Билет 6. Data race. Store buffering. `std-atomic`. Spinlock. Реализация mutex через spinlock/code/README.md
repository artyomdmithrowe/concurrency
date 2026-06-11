# Кодовые заготовки к билету 6

Папки пронумерованы как порядок рассказа на экзамене:

| Папка | Идея |
| --- | --- |
| `00_race_condition` | Логическая гонка без data race: атомарный баланс, но неатомарная операция `check-then-act` |
| `01_data_race_counter` | Обычный `++counter` из нескольких потоков ломается |
| `02_atomic_counter` | Исправление счётчика через `std::atomic::fetch_add` |
| `03_atomic_methods_cas` | `load`, `store`, `exchange`, `fetch_add`, CAS |
| `04_store_buffering` | Litmus-test: как возможен результат `(0,0)` |
| `05_store_buffering_fixed` | Исправление store buffering через `memory_order_seq_cst` |
| `06_naive_spinlock_bool` | Наивный spinlock через обычный `bool`: проверка и установка разделены |
| `07_spinlock_exchange` | Spinlock через `std::atomic<bool>::exchange` |
| `08_ttas_spinlock` | TTAS spinlock: сначала `load`, потом `exchange` |
| `09_atomic_flag_lock_free` | `std::atomic_flag` и `is_lock_free` |
| `10_spin_mutex` | Mutex-интерфейс поверх spinlock и использование через `std::lock_guard` |

## Быстрый маршрут

```bash
cd "Билеты/Билет 6. Data race. Store buffering. `std-atomic`. Spinlock. Реализация mutex через spinlock/code/00_race_condition"
./script.sh

cd ../01_data_race_counter
./script.sh

cd ../02_atomic_counter
./script.sh
```
