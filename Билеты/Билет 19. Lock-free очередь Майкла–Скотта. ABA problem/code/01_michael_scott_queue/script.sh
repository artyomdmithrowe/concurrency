#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -O2 -Wall -Wextra -pthread michael_scott_queue.cpp -o michael_scott_queue
./michael_scott_queue
