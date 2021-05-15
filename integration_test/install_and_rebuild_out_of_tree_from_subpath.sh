#!/bin/bash

echo "Install and rebuild out of tree (subath)"

install_dir_new=$(mktemp -d)
build_dir=temp_build_dir
trap "rm -rf $install_dir_new $build_dir" EXIT

make PREFIX="$install_dir_new" install
source_dir=..
mkdir $build_dir
cd $build_dir
$install_dir_new/bin/maike2 --source-dir=$source_dir --target-dir=__targets
__targets/core/bin/maike2 --version
cd $source_dir
