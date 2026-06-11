#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread read_once.cpp -luring -o read_once
./read_once
