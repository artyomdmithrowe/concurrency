#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread data_race_counter.cpp -o data_race_counter
./data_race_counter

