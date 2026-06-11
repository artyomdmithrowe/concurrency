#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread wait_timeout.cpp -o wait_timeout
./wait_timeout

