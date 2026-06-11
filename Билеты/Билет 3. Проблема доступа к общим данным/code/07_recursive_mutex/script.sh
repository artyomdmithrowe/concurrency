#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread recursive_mutex.cpp -o recursive_mutex
./recursive_mutex

