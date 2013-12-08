Systems Software: Assignment 06
===============================

This exercise was to use some server/client socket communication in C++.


Usage
-----

There are two binaries, one is called `server` and the other one `client`.

`server` CLI:

```sh
usage: server [absolute path to socket file]
```

`client` CLI:

```sh
usage: client [absolute path to socket file]
```

(This will also be printed to stdout if no sufficient parameters were given)

**Please note that the `server` executable must be initialized before starting the `client`!**


Build
-----

This project makes use of the `make` command.

To build the binary enter this command:

```sh
> make
```

If that command worked correctly you should find a binary called `server` and a binary called `client` in the same directory which is the output of the make.

Additionally you can remove all `*.o` files and all binaries if you enter this command:

```sh
> make clean
```


History
-------

* Initial commit /rko:rma, 2013-12-05


License
-------

MIT.
