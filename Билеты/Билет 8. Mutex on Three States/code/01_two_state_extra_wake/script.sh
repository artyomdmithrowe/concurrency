#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread two_state_extra_wake.cpp -o two_state_extra_wake
./two_state_extra_wake

