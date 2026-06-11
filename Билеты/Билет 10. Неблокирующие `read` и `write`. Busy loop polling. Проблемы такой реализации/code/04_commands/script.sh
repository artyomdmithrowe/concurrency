#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread \
  ../03_busy_loop_echo_server/busy_loop_echo_server.cpp \
  -o /tmp/busy_loop_echo_server_for_strace

strace -e trace=accept,read,write /tmp/busy_loop_echo_server_for_strace
