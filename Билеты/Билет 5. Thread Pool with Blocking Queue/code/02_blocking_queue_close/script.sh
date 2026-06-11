#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread blocking_queue_close.cpp -o blocking_queue_close
./blocking_queue_close

