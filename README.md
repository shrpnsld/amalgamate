# amalgamate

Combines C/C++ source files into one header and one source file.



# Usage

```bash
$ # Get it
$ git clone https://github.com/shrpnsld/amalgamate.git
```
```bash
$ # Use it
$ cd Source/Path/
$ Path/To/amalgamate [options]
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


### Options:

* `-I <header-path>` – path where header files can be found
* `-e <ext,ens,ions...>` – input header extensions
* `-s <ext,ens,ions...>` – input source extensions
* `-n <base-name>` – base name for output files
* `-o <output-dir-path>` – output path for `<base-name>-amalgamated/` directory
* `-x <hpp,cpp>` – extensions for output header and source files
* `-a` – insert annotations
* `-v` – verbose mode
* `-h` – show help message

While processing files, if script can't find header at path in `#include` directive, it searches header relative to the path passed to `-I` option. Default value for this option is current working directory's parent.

By default script uses parent directory name as base name for output header and source files. Output files are stored in `<base-name>-amalgamated/` directory at output path.

Default output path is current working directory's parent.

Extensions for output files are inferred from header and source files – if all extensions for headers/sources are the same then their extensions are used, otherwise `.h` and `.cpp` are used. To set extensions explicitly use `-x` option.