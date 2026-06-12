#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread collect_all_any.cpp -o collect_all_any
./collect_all_any
