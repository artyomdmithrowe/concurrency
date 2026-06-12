#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread one_shot_move_only.cpp -o one_shot_move_only
./one_shot_move_only
