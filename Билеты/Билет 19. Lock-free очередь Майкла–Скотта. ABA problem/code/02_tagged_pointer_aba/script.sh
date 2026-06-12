#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread tagged_pointer_aba.cpp -o tagged_pointer_aba
./tagged_pointer_aba
