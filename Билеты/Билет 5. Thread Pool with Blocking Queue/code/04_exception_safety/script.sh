#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread exception_safety.cpp -o exception_safety
./exception_safety

