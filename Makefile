all:
	./bootstrap

bootstrap: reset
	./make_bootstrap.sh

archive: bootstrap archive.sh
	./archive.sh __targets

reset:
	rm -rf __targets
	rm -f ./bootstrap
	rm -f ./build.sh

deb:	archive
	./debpack.sh __targets

DESTDIR?=""
.PHONY: install
install:
	./install $(DESTDIR)/usr
