#!/usr/bin/python3

#@	{
#@	 "targets":[{"name":"about.hpp"}]
#	,"dependencies":[{"ref":"../../vcslog.json"}]
#@	}

import sys

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		print(sys.argv[2], file=sys.stderr)
