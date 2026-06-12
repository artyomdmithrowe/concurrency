#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread complete_future.cpp -o complete_future
./complete_future
