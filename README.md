# The SimSoft Library
The _**Sim**on Bolivar & Company **Soft**ware **Library**_ is (intended to be) a cross-platform multilingual library containing various useful features to ease software development, such as:

* Container structures (vectors, hashmaps, queues, trees, ...)
* String-related functions & classes (strings, string views, regular expressions)
* Random number generators
* Generalized IO streams (similar to SDL's *RWops* or C++ *std::iostream*; read, write, seek, etc. for files, memory, structs, strings, etc.)
* Runtime dynamic library (i.e. shared object) loading

The library's base implementation is written in C11 [to provide a fun challenge for everyone **:^)** ].

## Goals
This library aims to be useful, customizable, heavy documented, and able to be integrated into a variety of programming languages without compromising their respective idioms/aesthetic (though C & C++ are currently the main priority).

Elaborating on the last point, we don't want the programmer to have to insert a bunch of ~~ugly~~ C code in the middle of an ~~ugly~~ C++ codebase. We'll take care of that for you; for example, the C++ variant of the library is planned to make use of the language's object oriented, RAII, references, operator overloading, and templating features.

For a more verbose and in-depth to-do list, see `TODO.txt` or visit our [Trello board](https://trello.com/b/ynDyCaWr/simsoft-library).

## Build
Install *gcc* (or *clang*) and *GNU make*, then type `make libsim` into a Bourne-compatible shell to build the library. Support for other compilers + Windows CMD.EXE will be added later on.

Various options exist to customize how the SimSoft library is built and assembled. Details on what options are available, along with other build targets, can be found by typing `make help` in the shell.
