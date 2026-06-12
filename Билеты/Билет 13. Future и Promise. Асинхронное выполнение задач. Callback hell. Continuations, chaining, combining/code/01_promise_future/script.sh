#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread promise_future.cpp -o promise_future
./promise_future
