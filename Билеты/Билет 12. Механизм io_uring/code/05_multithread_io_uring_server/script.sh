#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread multithread_io_uring_server.cpp -luring -o multithread_io_uring_server
./multithread_io_uring_server
