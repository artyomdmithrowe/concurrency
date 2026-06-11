#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread cv_semaphore.cpp -o cv_semaphore
./cv_semaphore

