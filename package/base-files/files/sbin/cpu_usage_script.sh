#!/bin/sh

top -b -n2 -d1 | grep "[C]PU:" | awk '{ sum += $8 } END { if (NR > 0) printf "%.2f\n", 100 - (sum / NR) }'
