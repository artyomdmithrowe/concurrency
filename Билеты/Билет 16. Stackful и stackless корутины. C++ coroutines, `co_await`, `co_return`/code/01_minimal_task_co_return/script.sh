#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread minimal_task_co_return.cpp -o minimal_task_co_return
./minimal_task_co_return
