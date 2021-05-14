#!/bin/bash

maike2 --target-dir=__targets
mv __targets/src/bin/maike2 maike2-tmp
tmp_dir=$(mktemp -d -t ci-XXXXXXXXXX)
repo_dir=$PWD
cd $tmp_dir
$repo_dir/maike2-tmp --source-dir=$repo_dir
rm -rf tmp_dir
rm $repo_dir/maike2-tmp