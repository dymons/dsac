DSAC: Data Structures, Algorithms and Concurrency
=================================================

What is `dsac`?
---------------

Dsac is a project containing the implementation of all the basic algorithms and data structures, as well as a bit of
concurrency. Dsac is focused on preparing for an interview, on forming the reader's full understanding of the internal
implementation of the algorithm and data structures.

Codemap
-------

`dsac` contains the following structure of the project

* [`algorithm`](dsac/include/dsac/algorithm) Algorithms
  <br /> The `algorithm` directory contains classic sorting algorithms. The main implementation is placed in the
  folder [`algorithm/detail`](dsac/include/dsac/algorithm/detail).
* [`container`](dsac/include/dsac/container) Data structure
  <br /> The `container` directory contains classic linear and non-linear data structure.
* [`concurrency`](dsac/include/dsac/concurrency) Concurrency algorithms and data structures
  <br /> The `container` directory contains concurrency primitives for writing asynchronous code efficiently.
* [`memory`](dsac/include/dsac/memory) Smart pointers and Allocators
  <br /> The `memory` directory contains implementation of smart pointers and different types of allocators.
* [`pattern`](dsac/include/dsac/pattern) Software design pattern
  <br /> The `pattern` directory contains implementation of common patterns to solve common problems in software design.
* [`examples`](dsac/examples) A set of examples on working with the POSIX API
  <br /> The `examples` directory contains examples with working POSIX API for writing backend apps using concurrency
  primitives from directory [`concurrency`](dsac/include/dsac/concurrency).

The current status of the project can be found at
the [whimsical.com/dsac](https://whimsical.com/dsac-JxDytPofZNmQLZdBoeC3jw)

Distributed Systems
-------------------

This repository also contains examples of the implementation of various algorithms and the theory of distributed systems
to illustrate the use of concurrency. Examples of the implementation of distributed algorithms can be found in the
folder with the prefix `examples/dist.*`

How to start?
-------------

To install `dsac` its dependencies and build this project, you just have to do:

Build the project

```bash
mkdir cmake-build-release && cd cmake-build-release
cmake .. && cmake --build .
```

License
-------

Dsac is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.