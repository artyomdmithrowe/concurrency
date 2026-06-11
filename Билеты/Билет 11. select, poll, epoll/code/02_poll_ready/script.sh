#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread poll_ready.cpp -o poll_ready
./poll_ready

