#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread nonblocking_write_buffer.cpp -o nonblocking_write_buffer
./nonblocking_write_buffer

