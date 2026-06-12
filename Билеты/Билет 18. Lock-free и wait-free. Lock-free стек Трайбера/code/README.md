# Кодовые заготовки к билету 18

Папки пронумерованы как порядок рассказа на экзамене:

| Папка | Идея |
| --- | --- |
| `01_treiber_stack` | Учебный Treiber stack: `push`/`pop` через CAS над `head` |
| `02_cas_contention` | CAS failures: lock-free даёт global progress, но поток может проигрывать |

## Быстрый маршрут

```bash
cd "Билеты/Билет 18. Lock-free и wait-free. Lock-free стек Трайбера/code/01_treiber_stack"
./script.sh

cd ../02_cas_contention
./script.sh
```
