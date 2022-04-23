
Install some dev packages on your VM:

    $ sudo apt install libbsd-dev clang-tools valgrind


## Hash Maps

Complete the provided top-triples program by implementing the implied “hashmap” data structure.

Your hashmap should use the "linear probing" strategy for collisions, and should have a hash function that indicates you put some thought into avoiding excess collisions.

-   top-triples: Find the top N letter triples occuring in words in the input, print the counts and triples to stdout.

Example input:

    abcd
    bcde
    cdef

Example session:

    $ ./top-triples input 2
    2   bcd
    2   cde

## Testing

Verify that your programs will pass the tests by running `perl test.pl`.

If you run into problems with the valgrind tests, make sure you are managing memory correctly. Any dynamically allocated object should be allocated with malloc exactly once, optionally reallocated many times, and then freed exactly once. It may be approprate to copy objects (allocate a new object and copy the contents) when a dynamically allocated value is inserted into a data structure to maintain a single “owner” for each object.
