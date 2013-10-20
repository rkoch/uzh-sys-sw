Systems Software: Assignment 02
===============================

This exercise was to learn about process creation and management in C++.

The goal was to write a program that, depending on user choice, performs one of the following two operations on a text file:

* Count the lines
* Search for a given string inside the file

The actual line count is done with the unix program `wc` and the search with `grep`. The main process in C++ spawns a child process with `fork()` - actually `vfork()` as the memory can be shared because it will be replaced by the `wc`/`grep` process through `execl()` anyways. The main process waits for the child to complete and returns a predefined exit code depending on the outcome of the process.


Usage
-----

The name of the binary is `fileops`.

```sh
usage: fileops file ...
```

(This will also be printed to stdout if no file was given)


Exit Codes
----------

* Regular termination:                        `0`
* Executed `wc` but exited with an error:     `1`
* Executed `grep` but could not find a match: `2`
* Executed `grep` but exited with an error:   `3`


Build
-----

This project makes use of the `make` command.

To build the binary enter this command:

```sh
> make
```

If that command worked correctly you should find a binary called `fileops` in the same directory which is the output of the make.

Additionally you can remove all `*.o` files and all binaries if you enter this command:

```sh
> make clean
```


History
-------

* Fixes return code problem when debugging in eclipse /rko, 2013-10-20
* Initial commit /rko:rma, 2013-10-07


License
-------

MIT.
