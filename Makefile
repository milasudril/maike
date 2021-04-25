.PHONY: all
all:
	maike

.PHONY: bootstrap
bootstrap:
	maike
	__targets/src/bin/maike_next --force-rebuild --dry-run > __targets/bootstrap
	chmod u+x __targets/bootstrap

.PHONY: clean
clean:
	rm -rf __targets*

.PHONY: format
format:
	find -name '*.hpp' -or -name '*.cpp' | xargs clang-format-10 -i
	find -name '*.json' -exec bash -c 'jq --tab --sort-keys . "{}" > .fmt && mv .fmt "{}"' \;
