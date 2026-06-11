#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread if_vs_while.cpp -o if_vs_while
./if_vs_while

