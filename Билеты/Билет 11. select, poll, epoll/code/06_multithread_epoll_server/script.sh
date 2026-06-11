#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread multithread_epoll_server.cpp -o multithread_epoll_server
./multithread_epoll_server

