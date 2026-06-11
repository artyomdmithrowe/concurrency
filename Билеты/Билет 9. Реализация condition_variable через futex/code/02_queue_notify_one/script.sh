#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread queue_notify_one.cpp -o queue_notify_one
./queue_notify_one

