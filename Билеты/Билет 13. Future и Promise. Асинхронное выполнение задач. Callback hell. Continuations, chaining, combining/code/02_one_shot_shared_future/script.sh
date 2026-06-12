#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread one_shot_shared_future.cpp -o one_shot_shared_future
./one_shot_shared_future
