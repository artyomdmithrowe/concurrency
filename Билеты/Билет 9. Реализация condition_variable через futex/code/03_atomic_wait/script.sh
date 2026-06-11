#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread atomic_wait.cpp -o atomic_wait
./atomic_wait

