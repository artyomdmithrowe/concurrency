#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread task_chaining_continuation.cpp -o task_chaining_continuation
./task_chaining_continuation
