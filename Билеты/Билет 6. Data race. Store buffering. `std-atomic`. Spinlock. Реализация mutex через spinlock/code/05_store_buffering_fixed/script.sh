#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread store_buffering_fixed.cpp -o store_buffering_fixed
./store_buffering_fixed

