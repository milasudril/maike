#!/bin/bash

echo "Bootstraping from $bootsrap_from_version"

tmp_dir=$(mktemp -d)
trap "rm -rf $tmp_dir" EXIT
cd $tmp_dir
wget "https://github.com/milasudril/maike/releases/download/$bootsrap_from_version/maike-src.tar.gz"
tar -xf maike-src.tar.gz
cd maike-src
sh -x __buildcache/bootstrap.sh
build_id_from_bootstrap=$(__targets_bootstrap/src/bin/maike2 --target-dir=__targets \
	| grep '#[[:space:]].*Id:' \
	| awk 'BEGIN { FS = ":" } ; { print $2 }' \
	| sed 's|[[:space:]]*||')

make PREFIX="$install_dir" install
build_id=$("$install_dir"/bin/maike2 --version \
	| grep 'Build id' \
	| awk 'BEGIN { FS = ":" } ; { print $2 }' \
	| sed 's|[[:space:]]*||')

if [[ "$build_id" == "$build_id_from_bootstrap" ]]; then
	exit 0
else
	echo "BuildId mismatch:"
	echo $build_id
	echo $build_id_from_bootstrap
	exit 1
fi