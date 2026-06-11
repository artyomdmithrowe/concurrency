#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread busy_wait_bad.cpp -o busy_wait_bad
./busy_wait_bad

