#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -O2 -Wall -Wextra -pthread shared_ptr_stack.cpp -o shared_ptr_stack
./shared_ptr_stack
