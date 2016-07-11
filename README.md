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

Project tree before `amalgamate`:

```bash
trace-out/
├── trace-out.hpp
└── detail
    ├── constants.hpp
    ├── function_printer.cpp
    ├── ... # 29 more file
    ├── posix
    │   ├── platform_specific.cpp
    │   ├── stdlib_specific.cpp
    │   └── ... # 5 more file
    └── windows
        ├── platform_specific.cpp
        ├── stdlib_specific.cpp
        └── ... # 5 more file
```


Project tree after `amalgamate`:

```bash
trace-out/
├── trace-out.hpp
└── trace-out.cpp
```


### Options:

* `-n <base_name>` – base name of the output files
* `-e <ext,ens,ions>` – header extensions
* `-s <ext,ens,ions>` – source extensions
* `-o <dir_path>` – output directory
* `-a` – insert annotations
* `-v` – verbose mode
* `-h` – show help message


By default the script uses *ParentFolderName* as base name for generated header and source files and stores them in the `./amalgamated/` folder.
