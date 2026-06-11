#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread manual_lock_problem.cpp -o manual_lock_problem
./manual_lock_problem

