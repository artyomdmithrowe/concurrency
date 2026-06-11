# Команды

```bash
nc 127.0.0.1 12000
```

```bash
nc 127.0.0.1 12001
```

```bash
ss -tnp sport = :12000 or dport = :12000
```

```bash
ss -tnp sport = :12001 or dport = :12001
```

```bash
pgrep -af io_uring_echo_server
```

```bash
pgrep -af multithread_io_uring_server
```

```bash
top -H -p <PID>
```

```bash
sudo strace -f -e trace=io_uring_enter,io_uring_register -p <PID>
```

```bash
strace -f -e trace=io_uring_setup,io_uring_enter,io_uring_register ./multithread_io_uring_server
```

```bash
man 7 io_uring
```

```bash
man 2 io_uring_setup
```

```bash
man 2 io_uring_enter
```

```bash
man 2 io_uring_register
```
