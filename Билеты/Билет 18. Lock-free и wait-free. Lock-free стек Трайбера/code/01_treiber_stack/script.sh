#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -O2 -Wall -Wextra -pthread treiber_stack.cpp -o treiber_stack
./treiber_stack
