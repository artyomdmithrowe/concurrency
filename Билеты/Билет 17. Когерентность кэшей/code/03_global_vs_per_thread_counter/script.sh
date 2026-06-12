#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -O2 -Wall -Wextra -pthread global_vs_per_thread_counter.cpp -o global_vs_per_thread_counter
./global_vs_per_thread_counter
