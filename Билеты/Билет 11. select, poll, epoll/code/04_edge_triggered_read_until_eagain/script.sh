#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread edge_triggered_read_until_eagain.cpp -o edge_triggered_read_until_eagain
./edge_triggered_read_until_eagain

