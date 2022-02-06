Parallel mergesort implementation using Intel TBB.

Prerquisites:
This program requires:
a) CMAKE version 3.1 or higher
b) Intel TBB package to be statically linked, provide path to cmake.


Installation and build.

After unzipping the tar, please run latest cmake to generate the makefile:
Command: cmake -S . -B ./build/ -DTBB_SRC={"Source for including TBB libraries"}
From build directory issue make to generate executable.

Running the program:
Target executable name: Mergesort
Option 1: Input text from stdin : Call executable without any arguments.
Option 2: Input text from stored files: Call executable with filename
          The  input files must be copied to test directory.

Test Cases:
the test directory contains a few test cases that can be run with providing
filename(only) to the executable target.

Comments:
Parallelization:
Parallelization is achieved using "parallel_invoke" on mergesort_internal
method using Intel's TBB.

CMakeLists.txt includes a flag option :

add_definitions(-D_PARALLEL_SORT_)

Removing the flag would default to serial execution of
mergesort_internal.


A comparison of performance of serial and parallel versions were made, and it
was observed that parallel version performs better for larger inputs(> 10,000
words, and lower CPU core count < 32 ). This is inline with the studies done on Intel TBb such as below:
Characterizing and Improving the Performance of Intel Threading Building Blocks
https://parsec.cs.princeton.edu/publications/contreras08tbb.pdf
However TBB frees the developer from choosing the # of threads required to run the program.
it employs cycle stealing, which optimizes performance in actual runtime, instead of having a fix number of threads and waiting for some thread to finish.


