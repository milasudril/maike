#!/bin/bash

set -eo pipefail
dir=`mktemp -d`

abort()
	{
	set +e
	rm -r "$dir"
	exit -1
	}

trap 'abort' 0

version=`cat versioninfo-in.txt`
dir=`mktemp -d`
dir_target="$1"
cp "$dir_target"/maike-src.tar.gz "$dir"/maike_"$version".orig.tar.gz
pushd .
cd "$dir"
tar -xf "$dir"/maike_"$version".orig.tar.gz
cd maike-src
./debinit.py
debuild -us -uc

popd
cp "$dir"/* .. || :
rm -r "$dir"
trap : 0
