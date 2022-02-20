#!/bin/bash
set -eo pipefail

shopt -s expand_aliases

dir=$1

mkdir -p "$dir"/bin
mkdir -p "$dir"/lib/maike2/recipes
cp __targets/core/bin/maike2 "$dir"/bin
chmod o+rx "$dir"/bin/maike2
cp -r recipes/* "$dir"/lib/maike2/recipes
chmod o+rx "$dir"/lib/maike2/recipes/*