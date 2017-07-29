#!/bin/bash

abort()
	{
	set +e
	exit -1
	}

trap 'abort' 0
set -eo pipefail

dir_target="$1"

find . | grep -v '^.$' | grep -v '__.*' | grep -v 'gh-pages' | grep -v 'debian' \
	| grep -v '/\..*' | grep -v '.*\.blend1' \
	| tar '--transform=s,^\.,maike-src,' -czf "$dir_target"/maike-src.tar.gz -T -

trap : 0
