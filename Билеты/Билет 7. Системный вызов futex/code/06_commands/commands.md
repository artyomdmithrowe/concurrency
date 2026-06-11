# Команды

```bash
g++ -std=c++20 -Wall -Wextra -pthread \
  ../04_wait_loop_wake_all/wait_loop_wake_all.cpp \
  -o /tmp/wait_loop_wake_all_for_strace
strace -f -e futex /tmp/wait_loop_wake_all_for_strace
```

```bash
man 2 futex
```

```bash
pgrep -af wait_loop_wake_all
```

```bash
ps -T -p <PID>
```

```bash
ps -L -p <PID> -o pid,tid,stat,wchan,comm
```
