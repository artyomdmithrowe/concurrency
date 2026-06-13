# Команды

```bash
g++ -std=c++20 -O2 -S release_acquire_publication.cpp -o release_acquire_publication.s
```

```bash
g++ -std=c++20 -O2 -S store_buffering_relaxed.cpp -o store_buffering_relaxed.s
```

```bash
objdump -d ./release_acquire_publication
```

```bash
perf stat ./store_buffering_relaxed
```
