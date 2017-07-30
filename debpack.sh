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
dir_target="$1"
cp "$dir_target"/maike-src.tar.gz "$dir"/maike_"$version".orig.tar.gz
pushd .
cd "$dir"
tar -xf "$dir"/maike_"$version".orig.tar.gz
cd maike-src
./debinit.py

bold="\\033[1m"
normal="\\033[0"
echo -e "Do you want to push the package to a PPA? (Yes or \033[1mNo\033[0m) "
method=""
read method
if [[ "$method" == "Yes" ]]; then
	bzr init
	bzr add debian/*
	bzr commit -m"Created a debian package"
	bzr builddeb -S
	cd ../build-area
	read -p "Enter your Launchpad username" username
	bzr push lp:~$username/+junk/maike-package
	dput ppa:$username/maike maike_*.changes
else
	debuild -us -uc
	echo "A package has been created in the parent directory. You can test build the source package by using pbuilder-dist on the file maike_*.dsc"
fi

popd
cp "$dir"/* .. || :
rm -r "$dir"
trap : 0
