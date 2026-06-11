#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread io_uring_echo_server.cpp -luring -o io_uring_echo_server
./io_uring_echo_server
