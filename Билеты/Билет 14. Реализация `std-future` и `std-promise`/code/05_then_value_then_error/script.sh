#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread then_value_then_error.cpp -o then_value_then_error
./then_value_then_error
