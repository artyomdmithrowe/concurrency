#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread shared_mutex_cache.cpp -o shared_mutex_cache
./shared_mutex_cache

