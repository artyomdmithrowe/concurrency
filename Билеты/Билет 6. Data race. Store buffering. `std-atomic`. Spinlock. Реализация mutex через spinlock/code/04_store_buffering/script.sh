#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread store_buffering.cpp -o store_buffering
./store_buffering

