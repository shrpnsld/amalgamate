# amalgamate

Bash script for combining C/C++ source files into one header and one source file.

Typical usage for this utility is development of a library using convinient project structure, and generating 1-2 files for more convinient distribution and usage.



# Usage

```bash
$ cd Source/Path/
$ Path/To/amalgamate [options]
```

By default the script uses *ParentFolderName* as base name for generated header and source files and stores them in the `./amalgamated/` folder.


#### Options:

* `-n`, `--name` `<base_name>` - base name
* `-e`, `--headers` `<ext,ens,ions>` - header extensions
* `-s`, `--sources` `<ext,ens,ions>` - source extensions
* `-o`, `--output` `<dir_path>` - destination directory
* `-a`, `--annotate` - insert annotations
* `-v`, `--verbose` - verbose mode
* `-h`, `--help` - show help message


#### Example with [pretty_output](https://github.com/shrpnsld/pretty_output) project

Before:

```bash
pretty_output/
├── implementation/
│   ├── console.h
│   ├── constants.h
│   └── ... # 18 more file
├── posix/
│   ├── platform_specific.cpp
│   ├── stdlib_specific.cpp
│   └── ... # 5 more file
├── pretty_output.h
└── windows/
    ├── platform_specific.cpp
    ├── stdlib_specific.cpp
    └── ... # 5 more file
```


After:

```bash
pretty_output/
├── pretty_output.cpp
└── pretty_output.h
```
