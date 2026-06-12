#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread subscribe_state_machine.cpp -o subscribe_state_machine
./subscribe_state_machine
