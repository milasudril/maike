#!/bin/bash

cat << 'EOF' > bootstrap
#!/bin/bash
set -eo pipefail

shopt -s expand_aliases

CXX_VERSION=$('g++' -dM -E -x c++ /dev/null | grep __cplusplus | awk '{print $3}' | sed 's/L//g')
if [ "$CXX_VERSION" -lt "201103" ]; then
	echo "Current compiler defaults to $CXX_VERSION. Trying to enable C++11."
	alias g++='g++ -std=c++11'
fi
EOF

maike | sed "s/^'g++'/g++/" >> bootstrap
chmod u+x bootstrap
cp bootstrap build.sh
