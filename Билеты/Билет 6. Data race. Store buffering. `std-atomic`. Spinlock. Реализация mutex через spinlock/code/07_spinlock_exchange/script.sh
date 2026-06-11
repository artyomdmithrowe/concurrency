#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread spinlock_exchange.cpp -o spinlock_exchange
./spinlock_exchange

