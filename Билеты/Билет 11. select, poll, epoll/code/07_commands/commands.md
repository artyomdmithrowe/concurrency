# Команды

```bash
nc 127.0.0.1 11011
```

```bash
nc 127.0.0.1 11012
```

```bash
ss -tnp sport = :11011 or dport = :11011
```

```bash
ss -tnp sport = :11012 or dport = :11012
```

```bash
pgrep -af epoll_echo_server
```

```bash
pgrep -af multithread_epoll_server
```

```bash
top -H -p <PID>
```

```bash
strace -e trace=epoll_create1,epoll_ctl,epoll_wait,accept,read,write -p <PID>
```

```bash
man 2 select
```

```bash
man 2 poll
```

```bash
man 7 epoll
```
