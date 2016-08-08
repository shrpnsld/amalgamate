# amalgamate

Combines C/C++ source files into one header and one source file.



# Usage

```bash
$ git clone https://github.com/shrpnsld/amalgamate.git # Get it
```
```bash
$ cd Source/Path/
$ Path/To/amalgamate [options] # Use it
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

* `-e <ext,ens,ions...>` – input header extensions
* `-s <ext,ens,ions...>` – input source extensions
* `-n <base-name>` – base name for output files
* `-o <dir-path>` – output path for `<base-name>-amalgamated/` directory
* `-x <hpp,cpp>` – extensions for output header and source files
* `-a` – insert annotations
* `-v` – verbose mode
* `-h` – show help message


By default the script uses parent directory name as base name for output header and source files. Output files are stored in `<base-name>-amalgamated/` directory in output path.

Default extensions for output files are inferred from header and source files – if all extensions for headers/sources are the same then their extensions are used, otherwise `.h` and `.cpp` are used.