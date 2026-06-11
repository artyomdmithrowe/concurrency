#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread wait_loop_wake_all.cpp -o wait_loop_wake_all
./wait_loop_wake_all

