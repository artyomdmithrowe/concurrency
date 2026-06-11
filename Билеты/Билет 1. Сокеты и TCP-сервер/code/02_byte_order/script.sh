#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra byte_order.cpp -o byte_order
./byte_order

