#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -O2 -Wall -Wextra -pthread store_buffering_relaxed.cpp -o store_buffering_relaxed
./store_buffering_relaxed
