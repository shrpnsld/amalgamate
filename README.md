# amalgamate

Combines C/C++ source files into a single header, a single source, or a header-source pair.



# Usage

```bash
$ # Get it
$ git clone https://github.com/shrpnsld/amalgamate.git
```
```bash
$ # Use it
$ amalgamate [options] # amalgamate all files in current directory and their dependencies
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
* `-i <path>` – input directory (defalt: current working directory).
* `-I <path>` – include directory (default: input directory).
* `-e <ext,ens,ions...>` – input header extensions.
* `-s <ext,ens,ions...>` – input source extensions.
* `-o <path>` – output directory (default: current working directory).
* `-n <name>` – base name for output files (default: current working directory name).
* `-x <hpp,cpp>` – extensions for output header and source files.
* `-S` – amalgamate into a single source file.
* `-M` – amalgamate into a header-source pair.
* `-P <file>` – add this file on top of amalgamated header.
* `-p <file>` – add this file on top of amalgamated source.
* `-G <id>` or `-G ''` – Instead of `#pragma once` use include guard with macro `id` or generate macro from header name.
* `-g` – group uninlined headers on top.
* `-b <count>` – reduce consecutive blank lines to no more than `<count>`.
* `-t` – trim trailing whitespace.
* `-a` – insert annotations.
* `-v` – verbose mode.
* `-- <file> ...` specify files to amalgamate, rather than amalgamate everything in the input directory.
* `-h` – show help message.

### Tags

Some lines in source code may need additional rules for processing, which you can mark with tags in the comments `[amalgamate:<tag>]`:

* `leave` – do not process line and insert it as is into amalgamated file.
* `remove` – do not insert line into amalgamated file.
* `uncomment` – uncomment line in amalgamated file.


#### Example:

```c++
//#if defined(__unix__) // [amalgamate:uncomment]
    #include <cassert> // [amalgamate:leave]
    #include "some-debug-stuff.hpp" // [amalgamate:remove]
//#endif // [amalgamate:uncomment]
```



# Notes

Default extensions for input headers are .h, .hh, .hpp, .hxx, .h++, .tpp, .txx, .tpl, .ii, .ixx, .ipp, .inl. Default extensions for input sources are .c, .cc, .cpp, .cxx, .c++.

`-P` and `-p` are considered the same if amalgamating into single file. Typical usage – license or copyright information.

`-L` can be used to place commented defines that control amalgamated project behavior. In some cases this might be more convinient than changing parent project's settings.

`-G ''` will generate macro name by uppercasing header name, changing non-letter characters with `_` and adding suffix `_INCLUDED`. For example, `some-header.hpp` with have include guard macro `SOME_HEADER_HPP_INCLUDED`

While processing files, if script can't find header at path in `#include` directive, it searches header that is relative to the path passed to `-I` option. Default value for this option is current working directory's parent.

By default script uses parent directory name as base name for output header and source files. Output files are stored in `<base-name>-amalgamated/` directory at output path.

Default output path is current working directory's parent.

Extensions for output files are inferred from header and source files – if all extensions for headers/sources are the same then their extensions are used, otherwise `.h` and `.cpp` are used. To set extensions explicitly use `-x` option.
