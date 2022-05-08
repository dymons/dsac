DSAC: Data Structures, Algorithms and Concurrency
=================================================

What is `dsac`?
---------------

Dsac is a project containing the implementation of all the basic algorithms and data structures, as well as a bit of
concurrency. Dsac is focused on preparing for an interview, on forming the reader's full understanding of the internal
implementation of the algorithm and data structures.

How to start?
-------------

To install `dsac` its dependencies and build this project, you just have to do:

```bash
conan install . --install-folder cmake-build-release --build=missing
cmake . -DCMAKE_TOOLCHAIN_FILE=cmake-build-release/conan_toolchain.cmake
cmake --build .
```

License
-------

Dsac is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.