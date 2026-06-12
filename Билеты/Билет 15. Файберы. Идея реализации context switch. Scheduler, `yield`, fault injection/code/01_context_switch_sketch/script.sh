#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread context_switch_sketch.cpp -o context_switch_sketch
./context_switch_sketch
