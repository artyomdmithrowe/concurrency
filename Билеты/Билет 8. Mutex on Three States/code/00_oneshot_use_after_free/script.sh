#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread -fsanitize=address -O0 -g \
  oneshot_use_after_free.cpp -o oneshot_use_after_free

set +e
./oneshot_use_after_free
status=$?
set -e

echo "program exit status: $status"

