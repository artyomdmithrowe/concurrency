#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread atomic_shared_ptr_lifetime.cpp -o atomic_shared_ptr_lifetime
./atomic_shared_ptr_lifetime
