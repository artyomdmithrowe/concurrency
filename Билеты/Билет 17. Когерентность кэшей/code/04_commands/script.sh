#!/usr/bin/env bash
set -euo pipefail

getconf LEVEL1_DCACHE_LINESIZE
lscpu | grep -i cache
