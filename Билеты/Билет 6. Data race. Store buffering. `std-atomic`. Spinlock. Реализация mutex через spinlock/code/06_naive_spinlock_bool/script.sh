#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread naive_spinlock_bool.cpp -o naive_spinlock_bool
./naive_spinlock_bool

