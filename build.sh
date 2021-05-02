#!/bin/bash
set -eo pipefail

shopt -s expand_aliases

maike2 --target-dir=__targets || ./bootstrap.sh || maike
__targets/src/bin/maike2 --target-dir=__targets2 || __targets/src/bin/maike_next --target-dir=__targets2
rm -rf __targets
mv __targets2 __targets