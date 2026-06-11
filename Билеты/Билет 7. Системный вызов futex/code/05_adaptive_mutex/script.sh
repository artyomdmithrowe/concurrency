#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread adaptive_mutex.cpp -o adaptive_mutex
./adaptive_mutex

