#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread manual_awaiter.cpp -o manual_awaiter
./manual_awaiter
