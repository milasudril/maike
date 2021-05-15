#!/bin/bash

echo "Install and rebuild out of tree"

install_dir_new=$(mktemp -d)
build_dir=$(mktemp -d)
trap "rm -rf $install_dir_new $build_dir" EXIT

make PREFIX="$install_dir_new" install
source_dir=$(pwd)
cd $build_dir
$install_dir_new/bin/maike2 --source-dir=$source_dir
maike_targets/core/bin/maike2 --version
cd $source_dir