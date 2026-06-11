#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread generation_counter.cpp -o generation_counter
./generation_counter

