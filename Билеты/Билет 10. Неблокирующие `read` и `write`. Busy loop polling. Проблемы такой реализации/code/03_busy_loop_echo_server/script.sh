#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread busy_loop_echo_server.cpp -o busy_loop_echo_server
./busy_loop_echo_server

