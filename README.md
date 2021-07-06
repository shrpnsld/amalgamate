# amalgamate

Combines C/C++ source files into one header and one source file.



# Usage

```bash
$ # Get it
$ git clone https://github.com/shrpnsld/amalgamate.git
```
```bash
$ # Use it
$ amalgamate [options] # amalgamate all files in current directory and their dependencies
$ amalgamate [options] -- [file ...] # specify files for amalgamation
$ # files will be amalgamated with their dependencies
```

Project tree [before](https://github.com/shrpnsld/trace-out/tree/master) `amalgamate`:

```bash
trace-out/
├── trace-out.hpp
└── detail/
    ├── constants.hpp
    ├── function_printer.cpp
    ├── ... # 29 more file
    ├── posix/
    │   ├── platform_specific.cpp
    │   ├── stdlib_specific.cpp
    │   └── ... # 5 more file
    └── windows/
        ├── platform_specific.cpp
        ├── stdlib_specific.cpp
        └── ... # 5 more file
```


Project tree [after](https://github.com/shrpnsld/trace-out/tree/dist) `amalgamate`:

```bash
trace-out/
├── trace-out.hpp
└── trace-out.cpp
```


### Options

* `-I <header-path>` – path where header files can be found
* `-e <ext,ens,ions...>` – input header extensions
* `-s <ext,ens,ions...>` – input source extensions
* `-o <output-dir-path>` – output path for `<base-name>-amalgamated/` directory
* `-H` – amalgamate all files to a single header file
* `-S` – amalgamate all files to a single source file
* `-n <base-name>` – base name for output files
* `-x <hpp,cpp>` – extensions for output header and source files
* `-g` – group unexpanded headers on top
* `-E <count>` – do not allow more than `<count>` consecutive empty lines (default: 2)
* `-G <id> | -G ''` – use include guard in header instead of `#pragma once`. Use `<id>` as header id or generate it from header name if empty string was passed
* `-t` – trim trailing whitespace
* `-a` – insert annotations
* `-v` – verbose mode
* `-h` – show help message

### Tags

Some lines in source code may need additional rules for processing, which you can mark with tags in the comments `[amalgamate:<tag>]`:

* `leave` – leave line
* `remove` – remove line
* `uncomment` – uncomment line


#### Example:

```c++
//#if defined(__unix__) // [amalgamate:uncomment]
    #include <cassert> // [amalgamate:leave]
    #include "some-debug-stuff.hpp" // [amalgamate:remove]
//#endif // [amalgamate:uncomment]
```



# Notes

While processing files, if script can't find header at path in `#include` directive, it searches header that is relative to the path passed to `-I` option. Default value for this option is current working directory's parent.

By default script uses parent directory name as base name for output header and source files. Output files are stored in `<base-name>-amalgamated/` directory at output path.

Default output path is current working directory's parent.

Extensions for output files are inferred from header and source files – if all extensions for headers/sources are the same then their extensions are used, otherwise `.h` and `.cpp` are used. To set extensions explicitly use `-x` option.
