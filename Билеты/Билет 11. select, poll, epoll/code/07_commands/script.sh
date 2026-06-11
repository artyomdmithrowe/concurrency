#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread \
  ../05_edge_triggered_missed_event/edge_triggered_missed_event.cpp \
  -o /tmp/edge_triggered_missed_event_for_strace

strace -e trace=epoll_create1,epoll_ctl,epoll_wait,read \
  /tmp/edge_triggered_missed_event_for_strace
