# Ideas for a new maike

## Data structures

Build pipeline:

 * Dependency collection&lt;Dependency scan dependencies&gt;
 * Dependency collection&lt;Compilation dependencies&gt;
 * Dependency collection&lt;Linking dependencies&gt;
 * Dependency collection&lt;Execution dependencies&gt;

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