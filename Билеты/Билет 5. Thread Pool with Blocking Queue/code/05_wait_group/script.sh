#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread wait_group.cpp -o wait_group
./wait_group

