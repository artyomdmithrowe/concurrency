#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread uncontended_compare.cpp -o uncontended_compare
./uncontended_compare

