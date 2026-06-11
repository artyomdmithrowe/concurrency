#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra socket_types.cpp -o socket_types
./socket_types

