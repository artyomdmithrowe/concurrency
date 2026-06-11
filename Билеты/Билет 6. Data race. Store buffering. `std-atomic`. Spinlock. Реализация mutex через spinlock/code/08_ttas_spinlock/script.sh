#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread ttas_spinlock.cpp -o ttas_spinlock
./ttas_spinlock

