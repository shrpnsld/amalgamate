# amalgamate

Combines multiple C/C++ header and source files into a single header, a single source, or a header-source pair.

### Features

* Offers a lot of control over the amalgamation process, but does not require you to use all of it.
* Written with Bash 3.2, so it works even on macOS with its stock Bash installation.



# Usage

```bash
$ # Get it
$ git clone https://github.com/shrpnsld/amalgamate.git
```
```bash
$ # Use it
$ amalgamate [options] # amalgamate all files in current directory and their dependencies
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
* `-1 <path>` – put this file on top of amalgamation queue (path should be relative to the input directory).
* `-b <count>` – reduce consecutive blank lines to no more than `<count>`.
* `-t` – trim trailing whitespace.
* `-a` – insert annotations.
* `-v` – verbose mode.
* `-- <file> ...` – specify files to amalgamate, rather than amalgamate everything in the input directory.
* `-h` – show help message.

### Tags

Some lines in the source code may need additional rules for processing, which you can mark with tags in the comments `[amalgamate:<tag>]`:

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

Default extensions for input headers are *h*, *hh*, *hpp*, *hxx*, *h++*, *tpp*, *txx*, *tpl*, *ii*, *ixx*, *ipp*, *inl*. Default extensions for input sources are *c*, *cc*, *cpp*, *cxx*, *c++*.

`-P` and `-p` are considered the same if amalgamating into a single file. The typical usage for this option is to place license or copyright information.

`-L` can be used to place commented defines that control the amalgamated project's behavior. In some cases, this might be more convenient than changing the parent project's settings.

`-G ''` will generate the macro name by uppercasing the header name, changing non-letter characters with `_`, and adding the suffix `_INCLUDED`. For example, *some-header.hpp* will have the include guard macro `SOME_HEADER_HPP_INCLUDED`.

`-1` can be useful when you want the content of a certain file to appear before the content of any other file (but after the prelude, include guard, interlude, and include directives). The typical usage is to put the module interface as the first code the user will see in the amalgamated file.

When resolving the path inside the `#include` directive, it is first checked as relative to the current file being processed. If no file is found there, then that path is checked inside the include directory.

By default, amalgamate uses the current working directory's name as the base name for the output header and source files. The output files are stored in the *\<base-name\>-amalgamated/* directory at the output path.

Extensions for the output files are inferred from the header and the source files – if all extensions for headers/sources are the same, then their extensions are used; otherwise, *h* and *cpp* are used. To set extensions explicitly, use the `-x` option.
