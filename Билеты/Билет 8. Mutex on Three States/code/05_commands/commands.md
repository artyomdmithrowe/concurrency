# Команды

```bash
g++ -std=c++20 -Wall -Wextra -pthread \
  ../01_two_state_extra_wake/two_state_extra_wake.cpp \
  -o /tmp/two_state_extra_wake_for_strace
strace -e futex /tmp/two_state_extra_wake_for_strace
```

```bash
g++ -std=c++20 -Wall -Wextra -pthread \
  ../04_uncontended_compare/uncontended_compare.cpp \
  -o /tmp/uncontended_compare_for_strace
strace -e futex /tmp/uncontended_compare_for_strace
```

```bash
man 2 futex
```
