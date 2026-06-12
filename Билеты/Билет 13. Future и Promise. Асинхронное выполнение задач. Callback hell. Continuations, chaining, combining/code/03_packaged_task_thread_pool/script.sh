#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread packaged_task_thread_pool.cpp -o packaged_task_thread_pool
./packaged_task_thread_pool
