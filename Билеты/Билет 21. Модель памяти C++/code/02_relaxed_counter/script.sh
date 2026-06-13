#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -O2 -Wall -Wextra -pthread relaxed_counter.cpp -o relaxed_counter
./relaxed_counter
