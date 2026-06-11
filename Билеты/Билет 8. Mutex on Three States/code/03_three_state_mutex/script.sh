#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread three_state_mutex.cpp -o three_state_mutex
./three_state_mutex

