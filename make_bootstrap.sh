#!/bin/bash

cat << EOF > bootstrap
#!/bin/bash
set -eo pipefail

CXX_VERSION=$(g++ -dM -E -x c++ /dev/null | grep __cplusplus | awk '{print $3}' | sed 's/L//g')
if [ "$CXX_VERSION" -le "201103" ]; then
	echo "Trying to enable c++11"
	alias g++='g++ -std=c++11'
fi
EOF

maike >> bootstrap
chmod u+x bootstrap
cp bootstrap build.sh
