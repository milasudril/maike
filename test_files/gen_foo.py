#!/usr/bin/env python3

#@	{"targets":[{"name":"foo.gen.hpp"}, {"name":"bar.gen.hpp"}]}

import sys
import json

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		args = json.loads(sys.argv[2])
		for target in args['targets']:
			with open(target, 'wb') as f:
				pass
