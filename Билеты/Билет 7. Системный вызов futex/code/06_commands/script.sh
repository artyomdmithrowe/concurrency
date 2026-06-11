#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread \
  ../04_wait_loop_wake_all/wait_loop_wake_all.cpp \
  -o /tmp/wait_loop_wake_all_for_strace

strace -f -e futex /tmp/wait_loop_wake_all_for_strace
