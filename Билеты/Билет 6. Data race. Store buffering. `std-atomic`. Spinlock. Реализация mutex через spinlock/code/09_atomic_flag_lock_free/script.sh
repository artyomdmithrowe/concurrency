#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread atomic_flag_lock_free.cpp -latomic -o atomic_flag_lock_free
./atomic_flag_lock_free

