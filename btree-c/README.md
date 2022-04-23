# BTree

## BTree Overview

Short video describing BTrees:

[https://www.youtube.com/watch?v=C\_q5ccN84C8](https://www.youtube.com/watch?v=C_q5ccN84C8)


-   Put and get operations should have asymptotic complexity of O(log(n)).
-   The test program from HW04 never deletes, so you can skip the delete operation.
-   You should implement the tree as described in the header file, with 4 values and 5 children per node.

top-triples: Find the top N letter triples occurring in words in the input,
print the counts and triples to stdout.

Example input:
```
abcd
bcde
cdef
```

Example session:
```
$ ./top-triples input 2
2   bcd
2   cde
```

## Testing

Pass the tests by running `perl test.pl`.
