#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra client.cpp -o client

echo "Built ./client"

