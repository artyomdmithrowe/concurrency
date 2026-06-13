# Кодовые заготовки к билету 20

Папки пронумерованы как порядок рассказа на экзамене:

| Папка | Идея |
| --- | --- |
| `01_atomic_shared_ptr_lifetime` | `std::atomic<std::shared_ptr<T>>`: atomic load держит объект живым |
| `02_shared_ptr_stack` | Treiber-like stack на `std::atomic<std::shared_ptr<Node>>` |

## Быстрый маршрут

```bash
cd "Билеты/Билет 20. Memory reclamation в lock-free структурах/code/01_atomic_shared_ptr_lifetime"
./script.sh

cd ../02_shared_ptr_stack
./script.sh
```
