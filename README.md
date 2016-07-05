# maike

Maike is a build system based on tracking of source files. Indeed, it is more or less a re-implementation of Wand. The most important user-visible improvement over Wand is that Maike features a more robust topological sorting algorithm, that also detects cyclic dependencies. Other improvements includes a system configuration test syntax. This makes it possible to have support for different libraries on the same "platform".

## TODO:s

 * Clean-up code
 * Add support for importing old targets
