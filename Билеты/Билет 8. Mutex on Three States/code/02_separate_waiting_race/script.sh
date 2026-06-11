#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread separate_waiting_race.cpp -o separate_waiting_race
./separate_waiting_race

