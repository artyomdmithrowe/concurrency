#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread mutex_sleep_bad.cpp -o mutex_sleep_bad
./mutex_sleep_bad

