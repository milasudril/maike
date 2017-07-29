all:
	./bootstrap

bootstrap: reset
	echo "#!/bin/bash" > bootstrap
	maike > bootstrap
	chmod u+x bootstrap
	cp bootstrap build.sh

archive: bootstrap archive.sh
	./archive.sh __targets

reset:
	rm -rf __targets
	rm -f ./bootstrap
	rm -f ./build.sh

deb:	archive
	./debpack.sh

DESTDIR?=""
.PHONY: install
install:
	./install $(DESTDIR)/usr