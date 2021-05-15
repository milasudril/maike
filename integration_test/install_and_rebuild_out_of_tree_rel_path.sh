#!/bin/bash

echo "Install and rebuild out of tree (relative path)"

install_dir_new=$(mktemp -d)
build_dir=..
trap "rm -rf $install_dir_new" EXIT

make PREFIX="$install_dir_new" install
source_dir=$(basename $(pwd))
cd $build_dir
$install_dir_new/bin/maike2 --source-dir=$source_dir
maike_targets/core/bin/maike2 --version
cd $source_dir