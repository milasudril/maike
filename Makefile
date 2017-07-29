all:
	./bootstrap

bootstrap: clean
	maike > ./bootstrap
	chmod u+x ./bootstrap
	cp ./bootstrap ./build.sh

archive: bootstrap archive.sh
	./archive.sh __targets

clean:
	rm -rf __targets
	rm -f ./bootstrap
	rm -f ./build.sh
