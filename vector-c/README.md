
    $ sudo apt install libbsd-dev clang-tools valgrind

## Vectors

Complete the provided length-sort program by implementing the implied “svec” data structure and the “sort” function. Don’t use quicksort. I suggest using [gnome sort](https://en.wikipedia.org/wiki/Gnome_sort).

-   length-sort: Sort the provided input file by string length, shortest to longest and print the result to stdout.

You can find the places where changes are needed in the starter code by running the command:

      hw04$ grep -nH TODO *


## Testing

Verify that your programs will pass the tests by running `perl test.pl`.

If you run into problems with the valgrind tests, make sure you are managing memory correctly. Any dynamically allocated object should be allocated with malloc exactly once, optionally reallocated many times, and then freed exactly once. It may be approprate to copy objects (allocate a new object and copy the contents) when a dynamically allocated value is inserted into a data structure to maintain a single “owner” for each object.
