#!/bin/bash

install_dir_new=$(mktemp -d)
trap "rm -rf $install_dir_new __targets_2 __targets_3" EXIT

make PREFIX="$install_dir_new" install
$install_dir_new/bin/maike2 --target-dir=__targets_2
__targets_2/core/bin/maike2 --target-dir=__targets_3