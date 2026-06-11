#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread select_ready.cpp -o select_ready
./select_ready

