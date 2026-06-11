#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra server.cpp -o server

echo "Built ./server"

