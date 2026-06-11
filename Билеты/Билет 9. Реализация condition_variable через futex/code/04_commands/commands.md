# Команды

```bash
g++ -std=c++20 -Wall -Wextra -pthread \
  ../02_queue_notify_one/queue_notify_one.cpp \
  -o /tmp/queue_notify_one_for_strace
strace -f -e futex /tmp/queue_notify_one_for_strace
```

```bash
man 2 futex
```
