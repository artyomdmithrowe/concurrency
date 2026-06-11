# Команды

```bash
nc 127.0.0.1 11010
```

```bash
ss -tnp sport = :11010 or dport = :11010
```

```bash
pgrep -af busy_loop_echo_server
```

```bash
top -H -p <PID>
```

```bash
strace -e trace=accept,read,write -p <PID>
```

```bash
man 2 fcntl
```

```bash
man 2 read
```

```bash
man 2 write
```
