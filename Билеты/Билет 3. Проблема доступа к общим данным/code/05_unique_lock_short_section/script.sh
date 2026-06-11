#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread unique_lock_short_section.cpp -o unique_lock_short_section
./unique_lock_short_section

