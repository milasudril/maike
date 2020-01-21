# Ideas for a new maike

## Data structures

Build pipeline:

 * Dependency collection<Dependency scan dependencies>
 * Dependency collection<Compilation dependencies>
 * Dependency collection<Linking dependencies>
 * Dependency collection<Execution dependencies>
 * Source filename
 * Final target names

 * Current stage


Dependency:
 * Filename
 * Entity refernece


Dependency collection:
 * Dependencies
 * Ready counter


Pipeline stages:
 * enum{Scan, Compile, Link, Execute}


## Algorithm

When all dependencies in one stage is satisfied and there is an execution resource available, it
starts to execute. On successful, it moves to the next stage. The main thead will wait until all
executions has completed