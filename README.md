# maike

Maike is a build system based on tracking of source files. Indeed, it is more or less a re-implementation of Wand. The most important user-visible improvement over Wand is that Maike features a more robust topological sorting algorithm, that also detects cyclic dependencies. Other improvements includes a system configuration test syntax. This makes it possible to have support for different libraries on the same "platform".

Maike uses embedded JSON data to control the compilation process. The best guide on how to tag source files and describe a project is probably the Maike source itself.

## Compiling Maike

Before compiling maike, make sure that appropriate `libjansson` development files are available on the system. If Maike is not available, run the script `build.sh`. If Maike is available, just run your installed version of Maike from the source directory.


## TODO:s

 * Add interface for setting up logwriters so Maike can be used as a library for graphical programs
 * Clean-up code
 * Add support for importing old targets
