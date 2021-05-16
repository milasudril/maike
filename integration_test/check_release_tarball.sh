#!/bin/bash

set -e

echo "Check release tarball"

extract_dir=$(mktemp -d)
trap "rm -rf $extract_dir" EXIT

make clean
make source-archive
mv __targets/maike-src.tar.gz $extract_dir
source_dir=$(pwd)
cd $extract_dir
tar -xf maike-src.tar.gz
cd maike-src
__buildcache/bootstrap.sh
__targets_bootstrap/core/bin/maike2 --version
cd $source_dir