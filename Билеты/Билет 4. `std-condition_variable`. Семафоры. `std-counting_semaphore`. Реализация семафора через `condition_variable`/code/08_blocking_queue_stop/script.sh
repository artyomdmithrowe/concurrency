#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread blocking_queue_stop.cpp -o blocking_queue_stop
./blocking_queue_stop

