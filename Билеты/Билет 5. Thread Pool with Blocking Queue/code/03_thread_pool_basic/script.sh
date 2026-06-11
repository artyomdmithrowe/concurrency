#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread thread_pool_basic.cpp -o thread_pool_basic
./thread_pool_basic

