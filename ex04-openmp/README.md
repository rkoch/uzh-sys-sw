Systems Software: Assignment 04
===============================

This exercise was to learn about multithreading using the OpenMP API in C++.


Usage
-----

The name of the binary is `patt_rec`.

```sh
usage: patt_rec [data_matrix.txt] [pattern_matrix.txt] [mode: serial=0, parallel=1] [nrOfThreads]
```

(This will also be printed to stderr if no sufficient parameters were given)

The program creates a file called `occurences.txt` in the current working directory which contains
all matched patterns in the given input matrix. Additionally it will print the execution time and
the min/avg/max goodness of the matched patterns to stdout. This output may look like the following:

```sh
execution time: 0.148292s
min goodness: 0.0192
avg goodness: 1.55893
max goodness: 3.2016
```


Build
-----

This project makes use of the `make` command.

To build the binary enter this command:

```sh
> make
```

If that command worked correctly you should find a binary called `patt_rec` in the current working directory.

Additionally you can remove all `*.o` files and all binaries if you enter this command:

```sh
> make clean
```

For this assignment there is another `make` command available, which will remove the resulting file (`occurences.txt`) from your working directory after running `patt_rec`:

```sh
> make clean-output
```

Output format
-------------

The output file (`occurences.txt`) is in the following format:

```
[col] [row]
```

An example:

```
4 12
5 19
```


History
-------

* Fixed results, Fixed protection of critical sections, Some refactoring /rko:rma, 2013-11-27
* Initial commit /rko:rma, 2013-11-03


License
-------

MIT.
