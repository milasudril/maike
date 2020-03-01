#!/bin/bash

valgrind --leak-check=full -q --error-exitcode=255 --gen-suppressions=all --track-fds=yes "$1"
#&& \
#valgrind --tool=helgrind -q --error-exitcode=255 "$1"
