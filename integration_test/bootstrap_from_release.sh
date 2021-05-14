#!/bin/bash

echo "Bootstraping from $bootsrap_from_version"

tmp_dir=$(mktemp -d)
trap "rm -rf $tmp_dir" EXIT
cd $tmp_dir
wget "https://github.com/milasudril/maike/releases/download/$bootsrap_from_version/maike-src.tar.gz"
tar -xf maike-src.tar.gz
cd maike-src
sh -x __buildcache/bootstrap.sh
__targets_bootstrap/src/bin/maike2 --target-dir=__targets
make PREFIX="$install_dir" install