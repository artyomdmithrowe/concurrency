#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread wait_wake.cpp -o wait_wake
./wait_wake

