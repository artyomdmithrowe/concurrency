#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread user_data.cpp -luring -o user_data
./user_data
