#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread \
  ../04_uncontended_compare/uncontended_compare.cpp \
  -o /tmp/uncontended_compare_for_strace

strace -e futex /tmp/uncontended_compare_for_strace

