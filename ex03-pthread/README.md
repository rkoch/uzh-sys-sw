Systems Software: Assignment 03
===============================

This exercise was to learn about multithreading using pthreads in C++.


Usage
-----

The name of the binary is `matcher`.

```sh
usage: matcher [sequences.txt] [patterns.txt] [mode: serial=0, parallel=1] [nrOfThreads]
```

(This will also be printed to stdout if no sufficient parameter were given)


Build
-----

This project makes use of the `make` command.

To build the binary enter this command:

```sh
> make
```

If that command worked correctly you should find a binary called `matcher` in the same directory which is the output of the make.

Additionally you can remove all `*.o` files and all binaries if you enter this command:

```sh
> make clean
```

For this assignment there is another `make` command available, which will remove the resulting file (`occurences.txt`) from your working directory after running `matcher`:

```sh
> make clean-output
```


History
-------

* Initial commit /rko:rma, 2013-10-27


License
-------

MIT.
