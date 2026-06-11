#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread nonblocking_read.cpp -o nonblocking_read
./nonblocking_read

