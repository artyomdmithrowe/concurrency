#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread cache_line_layout.cpp -o cache_line_layout
./cache_line_layout
