#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -O2 -Wall -Wextra -pthread cas_contention.cpp -o cas_contention
./cas_contention
