#!/bin/bash

echo "Bootstraping from $bootsrap_from_version"

tmp_dir=$(mktemp -d)
trap "rm -rf $tmp_dir" EXIT
echo $tmp_dir
cd $tmp_dir
wget "https://github.com/milasudril/maike/releases/download/$bootsrap_from_version/maike-src.tar.gz"
tar -xf maike-src.tar.gz
cd maike-src
pwd
make
make PREFIX="$install_dir" install