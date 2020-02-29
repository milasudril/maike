#!/bin/bash

abort()
	{
	set +e
	exit -1
	}

trap 'abort' 0
set -eo pipefail

dir_target="$1"

cp "$dir_target"/externals.json externals-in.json

find . -mindepth 1 '(' -false -or -name '.*'\
	 -or -name 'debian' \
	 -or -name 'gh-pages' \
	 -or -name '__*'  ')' -prune -or -print \
	| tar '--transform=s,^\.,maike-src,' -czf "$dir_target"/maike-src.tar.gz -T -

trap : 0
