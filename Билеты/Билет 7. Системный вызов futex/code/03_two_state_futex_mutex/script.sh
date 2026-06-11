#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread two_state_futex_mutex.cpp -o two_state_futex_mutex
./two_state_futex_mutex

