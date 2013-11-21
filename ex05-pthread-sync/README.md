Systems Software: Assignment 05
===============================

This exercise was to use mutual exclusion with pthreads in C++.


Usage
-----

The name of the binary is `food_stand`.

```sh
usage: food_stand [num_of_visitors] [stand_attendant_delay_ms] [visitor_sleep_min_ms] [visitor_sleep_max_ms]
```

(This will also be printed to stdout if no sufficient or invalid - eg. ms less than zero -  parameters were given)


Build
-----

This project makes use of the `make` command.

To build the binary enter this command:

```sh
> make
```

If that command worked correctly you should find a binary called `food_stand` in the same directory which is the output of the make.

Additionally you can remove all `*.o` files and all binaries if you enter this command:

```sh
> make clean
```


Remarks
-------

* The assignment description states that the program should terminate on any keystroke. This
  is currently not possible as `cin` buffers until `enter` was hit. Actually there are several
  solutions to this problem but they depend on specific terminals and may not work somewhere
  else. The current state is, that the program only terminates on hitting the `enter` key.

* On termination the program will print a statistic to stdout about how many food samples each
  visitor consumed to validate if there is no one who has been starving.

* To prevent starvation, each visitor thread checks if there is food available before trying to
  enter the critical zone. If there is no food available only the stand attendant thread will
  enter the critical zone to restock food.


History
-------

* Initial commit /rko:rma, 2013-11-21


License
-------

MIT.
