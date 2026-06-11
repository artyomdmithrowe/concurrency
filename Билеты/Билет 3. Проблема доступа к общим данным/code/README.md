# Кодовые заготовки к билету 3

Папки пронумерованы как порядок рассказа на экзамене:

| Папка | Идея |
| --- | --- |
| `01_data_race_counter` | Сначала показываем проблему: общий счётчик без синхронизации |
| `02_mutex_counter` | Чиним через `std::mutex` и `std::lock_guard` |
| `03_manual_lock_problem` | Почему ручные `lock`/`unlock` хрупкие |
| `04_prime_cache_mutex` | Общий кэш с обычным `std::mutex` |
| `05_unique_lock_short_section` | `std::unique_lock` и раннее освобождение lock |
| `06_shared_mutex_cache` | `std::shared_mutex`: много читателей, редкие записи |
| `07_recursive_mutex` | Что делает `std::recursive_mutex` и почему он спорный |
| `08_deadlock_scoped_lock` | Deadlock при разном порядке lock и исправление через `std::scoped_lock` |

## Быстрый маршрут

```bash
cd "Билеты/Билет 3. Проблема доступа к общим данным/code/01_data_race_counter"
./script.sh

cd ../02_mutex_counter
./script.sh
```

Дальше можно идти по номерам папок.
