.PHONY: all
all:
	./build.py

.PHONY: bootstrap
bootstrap:
	./build.py
	__targets/core/bin/maike2 --target-dir=__targets_bootstrap --num-workers=1 --force-rebuild --dry-run --log-level=compilation_command --build-id=0000000000000000000000000000000000000000000000000000000000000000 > __buildcache/bootstrap.sh
	chmod u+x __buildcache/bootstrap.sh

.PHONY: bootstrap-test
bootstrap-test: bootstrap
	__buildcache/bootstrap.sh
	rm -rf __targets
	__targets_bootstrap/core/bin/maike2

.PHONY: source-archive
source-archive: bootstrap
	rm -f __targets/maike-src.tar.gz
	(find __buildcache -type d; git ls-files) \
	| tar --xform s:'\(.*\)':'maike-src/\1': -czf __targets/maike-src.tar.gz -T -

DESTDIR?=""
PREFIX?="/usr"
.PHONY: install
install:
	./install.sh $(DESTDIR)$(PREFIX)

.PHONY: clean
clean:
	rm -rf __targets*

.PHONY: format
format:
	find -name '*.hpp' -or -name '*.cpp' | xargs clang-format-10 -i
	find -name '*.json' -exec bash -c 'jq --tab --sort-keys . "{}" > .fmt && mv .fmt "{}"' \;
