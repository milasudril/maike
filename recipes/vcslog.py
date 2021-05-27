#!/usr/bin/python3

import pygit2
import datetime
import sys
import json
import os

def collect_commits(repo):
	ret = []
	last = repo[repo.head.target]
	for commit in repo.walk(last.id, pygit2.GIT_SORT_TIME):
		shortmsg = commit.message.split('\n')[0]
		tag = repo.describe(committish=commit.hex,
			show_commit_oid_as_fallback=True,
			dirty_suffix='-dirty')
		ret.append([commit.author.time, shortmsg, tag])

	return ret

def get(target, cache):
	try:
		ret = dict()
		repo = pygit2.Repository('.git')
		ret['commit'] = repo.revparse_single('HEAD').hex
		ret['tag'] = repo.describe(show_commit_oid_as_fallback=True, dirty_suffix='-dirty')
		ret['changelog'] = collect_commits(repo)
		return ret
	except:
		with open(cache) as f:
			return json.load(f)

def mkdir(path):
	try:
		os.mkdir(path)
	except FileExistsError:
		pass

if __name__ == '__main__':
	try:
		if sys.argv[1] == 'compile':
			data = json.loads(sys.argv[2])
			target = data['targets'][0]
			src_dir = data['build_info']['source_dir']
			cache = src_dir + '/__buildcache/vcslog.json'
			data = get(target, cache)
			with open(target, 'w') as f:
				json.dump(data, f)
			mkdir(src_dir + '/__buildcache')
			with open(cache, 'w') as f:
				json.dump(data, f)
	except Exception as e:
		print(e, file=sys.stderr)
		exit(-1)