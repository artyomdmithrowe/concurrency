#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -O2 -Wall -Wextra -pthread false_sharing_benchmark.cpp -o false_sharing_benchmark
./false_sharing_benchmark
