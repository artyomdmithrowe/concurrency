#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread deadlock_scoped_lock.cpp -o deadlock_scoped_lock
timeout 3s ./deadlock_scoped_lock || echo "stopped by timeout: unsafe_transfer_* deadlocked"
