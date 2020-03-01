#!/bin/bash

valgrind --tool=helgrind -q --error-exitcode=255 --sim-hints=no-nptl-pthread-stackcache "$1"

#valgrind --leak-check=full -q --error-exitcode=255 --gen-suppressions=all --track-fds=yes "$1" && \
