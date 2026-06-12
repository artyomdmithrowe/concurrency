#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread shared_state_value.cpp -o shared_state_value
./shared_state_value
