#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread semaphore_guard.cpp -o semaphore_guard
./semaphore_guard

