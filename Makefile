.PHONY: all
all:
	./build.sh

.PHONY: bootstrap
bootstrap:
	./build.sh
	__targets/src/bin/maike2 --target-dir=__targets_bootsrap --num-workers=1 --force-rebuild --dry-run --log-level=compilation_command > __targets/bootstrap.sh
	chmod u+x __targets/bootstrap

.PHONY: bootstrap-test
bootstrap-test: bootstrap
	__targets/bootstrap.sh
	rm -rf __targets
	__targets_bootsrap/src/bin/maike2

.PHONY: clean
clean:
	rm -rf __targets*

.PHONY: format
format:
	find -name '*.hpp' -or -name '*.cpp' | xargs clang-format-10 -i
	find -name '*.json' -exec bash -c 'jq --tab --sort-keys . "{}" > .fmt && mv .fmt "{}"' \;
