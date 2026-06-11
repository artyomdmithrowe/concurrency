#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread race_condition.cpp -o race_condition
./race_condition

