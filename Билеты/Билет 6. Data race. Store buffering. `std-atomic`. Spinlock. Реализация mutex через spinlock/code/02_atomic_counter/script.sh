#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread atomic_counter.cpp -o atomic_counter
./atomic_counter

