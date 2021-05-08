#!/usr/bin/python3

#@	{
#@	 "targets":[{"name":"vcslog.json"}]
#@	}

import pygit2
import datetime
import sys
import json

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

def get():
	try:
		ret = dict()
		repo = pygit2.Repository('.git')
		ret['commit'] = repo.revparse_single('HEAD').hex
		ret['tag'] = repo.describe(show_commit_oid_as_fallback=True, dirty_suffix='-dirty')
		ret['changelog'] = collect_commits(repo)
		return ret
	except:
		with open('vcslog.json') as f:
			return json.load(f)

if __name__ == '__main__':
	if sys.argv[1] == 'compile':
		target = json.loads(sys.argv[2])['targets'][0]
		data = get()
		with open(target, 'w') as f:
			json.dump(data, f)