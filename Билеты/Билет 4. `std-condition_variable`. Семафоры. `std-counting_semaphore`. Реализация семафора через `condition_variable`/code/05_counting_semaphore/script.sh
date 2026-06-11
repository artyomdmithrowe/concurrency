#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread counting_semaphore.cpp -o counting_semaphore
./counting_semaphore

