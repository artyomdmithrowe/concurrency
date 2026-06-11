#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread edge_triggered_missed_event.cpp -o edge_triggered_missed_event
./edge_triggered_missed_event

