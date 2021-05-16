#!/bin/bash

set -e

echo "Building new version"

rm -rf __targets*
build_id_expected=$($install_dir/bin/maike2 --target-dir=$target_dir \
	| grep '#[[:space:]].*Id:' \
	| awk 'BEGIN { FS = ":" } ; { print $2 }' \
	| sed 's|[[:space:]]*||')
version_info=$(__targets/core/bin/maike2 --version)
build_id=$(echo "$version_info" \
	| grep 'Build id' \
	| awk 'BEGIN { FS = ":" } ; { print $2 }' \
	| sed 's|[[:space:]]*||')
commit_expected=$(git rev-parse HEAD)
commit=$(echo "$version_info" \
	| grep '# Maike' \
	| tr ')' '(' \
	| awk 'BEGIN { FS = "(" } ; { print $2 }')

if [[ "$build_id" == "$build_id_expected" ]]; then
	exit 0
else
	echo "BuildId mismatch:"
	echo $build_id
	echo $build_id_expected
	exit 1
fi

if [[ "$commit" == "$commit_expected" ]]; then
	exit 0
else
	echo "BuildId mismatch:"
	echo $commit
	echo $commit_expected
	exit 1
fi
