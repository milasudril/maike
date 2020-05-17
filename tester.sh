#!/bin/bash
export test_exe="$1"
valgrind --leak-check=full -q --error-exitcode=255 --gen-suppressions=all --track-fds=yes "$1" && \
valgrind --tool=helgrind -q --error-exitcode=255 --gen-suppressions=all --suppressions=./helgrind.sup --sim-hints=no-nptl-pthread-stackcache "$1"

