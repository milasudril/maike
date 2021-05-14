#!/bin/bash

echo "Can generate working boostrap script"

rm -rf __targets_bootstrap __buildcache
mkdir __buildcache
"$1"  --target-dir=__targets_bootstrap \
	--num-workers=1 --force-rebuild \
	--dry-run \
	--log-level=compilation_command \
	--build-id=0000000000000000000000000000000000000000000000000000000000000000 > __buildcache/bootstrap.sh
sh -x __buildcache/bootstrap.sh
__targets_bootstrap/src/bin/maike2 --about