# C Build Program++

## Synopsis

This is a program that is used to build C/C++ (CXX) projects via C++. It is a drop in, plug-and-play 1-file solution. Unlike CMake, this program does not require a separate build directory and no custom DSL.

It is plain C++ with no dependencies other than the [`backend/`](backend/) C files and the standard C++ library (libc++).

## Installation

Just clone this repo and make sure your project looks like...
```
├── src/
│   ├── main.cpp
│   └── ...
├── build/
│   └── Makefile # or other artifacts
├── build.cpp    # <-- important
...
```

With no other `.cpp` files in the project's root, other than `build.cpp`.

## Usage

1. Simply, first write out your build.cpp file (MUST)...
A simple one looks like
```cpp
#include "vendor/cbp/src/cbp.hpp"

void build(int argc, char** argv) {
    CBuildP::file_t input_files = CBuildP::get_files(argc, argv); // use the cmdline
    CBuildP::file_t output_file = "bin/executable";

    CBuildP::optimize({
        .compiler = CBuildP::compilers::c::clang,
        .level = 3, /* maximum optimization */
        .debug = true /* generate debug symbols so that no printf logging */
    });
    CBuildP::specs(input_files, output_file);
    CBuildP::compile(input_files, output_file);
}
```

A nice complex one looks like this (with inclusion, headers, libraries, l and i commands).
Below is an example of a complex build script (building a calculator executable that is conf'ed
using lua).
```cpp
#include "vendor/cbp/src/cbp.hpp"

void build(int argc, char** argv) {
    CBuildP::file_t input_files = CBuildP::get_files(argc, argv);
    CBuildP::file_t output_file = "build.exec.calc";

    CBuildP::optimize({
        .compiler = CBuildP::compilers::cxx::clang,
        .level = CBuildP::optimization::max,
        .debug = true
    });
    CBuildP::specs(input_files, output_file);
    CBuildP::compile_flags({
        CBuildP::execcmd("pkg-config --cflags gtk+-3.0 lua5.4") /* get lua and gtk flags */
    });
    CBuildP::compile_link({
        CBuildP::execcmd("pkg-config --libs gtk+-3.0 lua5.4") /* get lua and gtk libs */
    });
    CBuild::link({
        /* libraries here */
    }, {
        /* lookup directories here */
    });
    CBuildP::compile(input_files, output_file);
}
```

2. Then, build via running `./bin/bpp build` and `./cbp FILES` or `./cbp`
```terminal output
[yyarch@archlinux zed]$ ./bin/bpp build
:: Building with command
./bin/bpp build
:: Optimization stats...
   - opt level: 1
   - debug: 1
:: Optimizing the standard ...
:: DONE
:: Compiling: build.cpp stdstr.o
In file included from build.cpp:1:
In file included from ./backend/lib/cbp.cpp:12:
./backend/lib/colors.c:5:9: warning: 'CEN__STR_ARENA_SIZE' macro redefined [-Wmacro-redefined]
    5 | #define CEN__STR_ARENA_SIZE 32
      |         ^
./backend/lib/../cstr.h:19:9: note: previous definition is here
   19 | #define CEN__STR_ARENA_SIZE 4096
      |         ^
1 warning generated.

:: Compilation successful
 CODE 0
:: Build finished
Build output to cbp
[yyarch@archlinux zed]$ ./cbp src/main.cpp
:: Building with command
./cbp src/main.cpp
:: Optimization stats...
   - opt level: 3
   - debug: 1
:: Optimizing the standard ...
:: DONE
:: Build info:
   - C/C++ files : src/main.cpp
   - output file : build.exec.calc
   - optimization: 3
   - debug       : 1
   - compiler    : clang++
:: Compiling (with linkage): src/main.cpp

:: Compilation successful
 CODE 0
:: Build finished
[yyarch@archlinux zed]$
```

3. Run the executable. Here in the above output...  run `./build.exec.calc`

## Docs

There is currently no documentation (mandoc) available _yet_.
