#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra -pthread epoll_echo_server.cpp -o epoll_echo_server
./epoll_echo_server

