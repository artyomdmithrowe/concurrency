#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread prime_cache_mutex.cpp -o prime_cache_mutex
./prime_cache_mutex

