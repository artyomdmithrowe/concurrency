#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -O2 -Wall -Wextra -pthread release_acquire_publication.cpp -o release_acquire_publication
./release_acquire_publication
