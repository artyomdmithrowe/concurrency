#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread exception_broken_promise.cpp -o exception_broken_promise
./exception_broken_promise
