#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++20 -Wall -Wextra atomic_methods_cas.cpp -o atomic_methods_cas
./atomic_methods_cas

