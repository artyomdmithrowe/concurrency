#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread ring_basics.cpp -luring -o ring_basics
./ring_basics
