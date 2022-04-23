## Concept: Sample Sort

Sample sort is a parallel version of quicksort.

It works as follows:

Input:

-   An array of N items
-   An integer P for number of threads to use.

Result:

-   An array of N items, sorted.

1.  Setup

-   Read input file into a "floats" in memory.
-   Allocate an array, sizes, of P longs.
-   Use open / ftruncate to create the output file, same size as the input file.

2.  Sample

-   Select 3\*(P-1) items from the array.
-   Sort those items.
-   Take the median of each group of three in the sorted array, producing an array (samples) of (P-1) items.
-   Add 0 at the start and +inf at the end (or the min and max values of the type being sorted) of the samples array so it has (P+1) items numbered (0 to P).

2.  Partition

-   Spawn P threads, numbered p in (0 to P-1).
-   Each thread builds a local array of items to be sorted by scanning the full input and taking items between samples\[p\] and samples\[p+1\].
-   Write the local size to sizes\[p\].

4.  Sort locally

-   Each thread uses quicksort to sort the local array.

5.  Write the data out to the file.

-   Open a separate file descriptor to the output file in each thread.
-   Use lseek to move to the correct location to write in the file.

6.  Cleanup

-   Terminate the P subthreads.

## Tasks

### Task 1: Write a Program

-   Your program, ssort, should take three arguments:
-   An integer specifying the number of threads to sort with
-   The input file
-   The output file
-   The data files are structured as follows:
-   First 8 bytes: long size - number of floats in the file
-   Next 4\*size bytes: An array of doubles
-   Use read/write for file IO
-   Use sample sort to sort the data from the input file
-   Write the results to the output file

In addition, each subthread should print out some text, showing that it's actually participating in sample sort:

     {p}: start {start}, count {length of local array}  

Example session:

    $ ./tools/gen-input 20 data.dat
    $ ./ssort 4 data.dat data1.dat
    0: start 0.0000, count 3
    1: start 7.5690, count 5
    2: start 27.1280, count 11
    3: start 95.5110, count 1
    $ ./tools/check-sorted data1.dat
    $

### Task 2: Measure Parallel Speedup

Test your sample sort program in six scenarios, each with a newly generated input file of 50 million floats (or enough to take over 10 seconds for 1 process on your machine):

-   Your local machine, 1 process
-   Your local machine, 4 processes
-   Your local machine, 8 processes
-   The CCIS server, 1 process
-   The CCIS server, 4 processes
-   The CCIS server, 8 processes

_DO NOT_ run your program on the CCIS server until you've completed the timing process locally and confirmed that your program runs in a reasonable amount of time and terminates properly.

Time these tests using the "/usr/bin/time -p" command. (You're interested in "real" time)

Repeat each test three times, taking the median.

You should get a decent parallel speedup. If you're not getting a good parallel speedup, consider how you can improve your program to make it work better in parallel.

### Task 3: report.txt

Create a file, report.txt, containing the following information:

-   The operating system and number of processor cores of your test machines.
-   A table of measured time and parallel speedup for each test.
-   Parallel speedup is calculated as (time for 1 thread / time for N threads)
-   Optimal parallel speedup is N.
-   A paragraph or two discussing what results you got and why you got them.
-   How do you results compare to the process / mmap version? Why?

Make a graph (graph.png) showing the measured time for each case.

## To Submit

-   A .tar.gz archive with exactly one top level directory containing:
-   Your source code, including a Makefile that builds the tssort binary.
-   Either the tools directory, source and Makefile only, unmodified, or no tools directory.
-   A report.txt (which should be plain Unix text, line wrapped to 70 columns)
-   A graph.png (which should be a PNG image)
-   No binaries, object files, or input/output data files.
