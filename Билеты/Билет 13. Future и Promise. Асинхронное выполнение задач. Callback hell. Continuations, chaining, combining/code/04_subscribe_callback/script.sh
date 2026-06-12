#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread subscribe_callback.cpp -o subscribe_callback
./subscribe_callback
