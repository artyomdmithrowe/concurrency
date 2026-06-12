#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread executor_via.cpp -o executor_via
./executor_via
