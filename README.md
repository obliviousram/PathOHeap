# README

All source files are in the `src/` directory.
- `Block`, `Bucket`: contains implementation for the Block and Bucket data structures as described
  in the paper.
- `UntrustedStorageInterface`: defines the interface that the Path OHeap implemenation uses to
  communicate with the untrusted cloud storage provider. A possible implementation is defined in
  `ServerStorage`.
- `RandForOHeapInterface`: the Path OHeap implementation gets random leaf ids from here. A possible
  implementation is defined in `RandomForOHeap`. Can be overridden e.g. for testing purposes.
- `OHeapInterface`: The interface with which to use this Path OHeap implementation.
  `OHeapReadPathEviction` contains the reference implementation itself.
- `Job`, `Job2`: Very simple runtime demonstrations of the Path OHeap.
- `JobForStatisticalSimulation`: Practically examines the stash size (root bucket size) argument
  from the paper, performing 500 million operations on a single instance and plotting how often the
  stash size exceeds certain values.
- `PlainHeapComparisonTest`: A correctness test that performs operations at random on both an
  instance of this Path OHeap and a 'plain' C++ heap, ensuring that they always produce the same
  results.
- `main`: Runs one of the above four tests.

To compile and run: 

1. Uncomment out the desired test in `src/main.cpp`, comment out any other tests. 
2. Run `make` in this directory. 
3. Run the test with `./POHeapTester`.

C++17 is required. Known to compile with `gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1)` and `gcc version 7.3.1 20180130 (Red Hat 7.3.1-2) (GCC)`.
