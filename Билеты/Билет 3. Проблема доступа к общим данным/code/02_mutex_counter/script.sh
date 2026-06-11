#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread mutex_counter.cpp -o mutex_counter
./mutex_counter

