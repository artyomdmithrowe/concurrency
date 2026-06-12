#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread then_value_chaining.cpp -o then_value_chaining
./then_value_chaining
