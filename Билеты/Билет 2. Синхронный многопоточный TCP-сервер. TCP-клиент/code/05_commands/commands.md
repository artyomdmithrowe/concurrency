# Commands

```bash
g++ -std=c++20 -Wall -Wextra -pthread server.cpp -o thread_server
./thread_server
nc localhost 11000
g++ -std=c++20 -Wall -Wextra client.cpp -o client
./client
pgrep -x thread_server
ps -T -p <pid>
ls /proc/<pid>/task
ss -tn sport = :11000 or dport = :11000
```
