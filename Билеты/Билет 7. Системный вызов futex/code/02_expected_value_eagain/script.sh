#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread expected_value_eagain.cpp -o expected_value_eagain
./expected_value_eagain

