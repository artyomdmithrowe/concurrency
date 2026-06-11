#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread server.cpp -o thread_server

echo "Built ./thread_server"
