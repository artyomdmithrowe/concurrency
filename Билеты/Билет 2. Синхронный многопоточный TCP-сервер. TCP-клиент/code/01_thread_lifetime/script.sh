#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread thread_lifetime.cpp -o thread_lifetime
./thread_lifetime

