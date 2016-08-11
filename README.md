# Maike

Maike is a build system based on scanning of source files. To construct the dependency graph, Maike uses embedded JSON data to control the compilation process. The best guide on how to tag source files and describe a project is probably the Maike source itself.

## System requirements

Maike requires requires currently relies on a GNU system, with glibc version 2.3.4 or later, running on the Linux® kernel, version 2.6.33 or later. Also, it requires `libjansson`, and the GNU C++ compiler version 5 or later. It is likely that Clang also works, but it has not been tested. There are plans for porting the system to Windows and MinGW, but MSVC is not being considered.


## Obtaining Maike

The source for the latest Maike can be obtained by cloning the git repository.

	git clone https://github.com/milasudril/maike

The latest release version can be found at [the github release page][1]. The git repository may contain features not present in the release version, but may require an up-to-date version of Maike in order to be compiled. The release package always contains a build script that can be used for bootstrapping.

Before compiling maike, make sure that appropriate `libjansson` development files are available on the system. If Maike is not available, or the installed version is to old, run the script `build.sh` within the source directory.

	./build.sh

Otherwise, just run your installed version of Maike from the source directory.

	maike

If the compilation succeeded, the Maike executable should be present in the newly created `__targets` directory. In order to use it, copy it to a directory mentioned in your `PATH` variable. For example

	cp __targets/maike ~/bin/maike

It is *very* likely that you need the additional target hooks. These reside inside two shared objects (dll:s)

	cp __targets/*.so ~/bin/maike

A complete installation can be done by running the supplied install script.

    sudo ./install.sh

By default, it copies stuff to `/usr/local`. If not root, or if want to install Maike somewhere else anyways, you can change that by using the `--prefix` option.

    ./install.sh --prefix=/home/alice

To uninstall Maike

	./uninstall.sh

## Using Maike

More information on how to use Maike can be found on [the Maike homepage][2].


## TODO:s

See [Issues][3]

 [1]: https://github.com/milasudril/maike/releases/latest
 [2]: https://milasudril.github.io/maike
 [3]: https://github.com/milasudril/maike/issues

