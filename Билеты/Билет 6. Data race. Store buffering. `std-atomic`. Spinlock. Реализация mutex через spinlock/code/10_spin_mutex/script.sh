#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread spin_mutex.cpp -o spin_mutex
./spin_mutex

