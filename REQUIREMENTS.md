# Requirements

Register with the shell to handle URIs described in [rfc3986][1]. Only part 
of the RFC needs to be implemented, the following parts are recognized:
proto:[//]authority[/path][?query][#fragment].

## URI Parts
| rfc3986 | function                             |
|---------|--------------------------------------|
| proto   | used by the shell to map to this exe |
| authority | used to define the action |
| path | used to define the sub action |
| query | holds all parameters for the executable |
| fragment | placeholder |

## Mapping URI Parts to configuration
URI Parts are mapped to actions in a configuration file. The configuration file 
is in the common `ini` format.

The concatenated `authority` and `path`part make up the ini file section.
For example, `proto:aaa/bbb` would be mapped to the section `[aaa/bbb]`
in the ini file.

The `query`part is used to pass parameters to the executable. Special care
has to be taken when pasing in path information. 

## Ini file directives

### base_path
All paths passed in as parameter must be within this base path. If a relative 
path is passed as parameter the base path will be treated as `cwd`. Relative 
paths may not start with `..`. A path which does not start with `[^A-Z:]`, 
`^/` and `^\.\.` is treated as relative path.

### mapping
Mapping query parameter names to command line parameters. Passing a query 
string `?file=somefile.txt` might need to be converted into `/f somefile.txt`
on the command line.

### replace_file
This directive only works together with `replace_regex`. This directive
contains the file name on which the text needs to be replaced.

### replace_regex
Defines a search and replace string as regular expression ([Regex][2]). The 
Format is written as used from sed, vi, etc. Example:
`/^(search)/replace \1/mi`.
See [Overview of Regular Expression Syntax][3] for more information on Posix regular expressions.

## Goals
- [ ] open documents
- [x] run executables
  - [ ] expand environment variables
  - [ ] prepend parameters
  - [ ] append static parameters
- [ ] map url parmeter names to cmd parameter names
- [ ] replace text in config files
  - [ ] implement regex modifiers
- [ ] sanitize and check paths
- [ ] logging (configurable)
- [ ] checking if parameter passed username equals %USERNAME%
- [ ] display error messages if something goes south
- [ ] global config (log path, log level)
  - [ ] logging
  - [ ] global exe wrappers (ie. prefix `cmd /c ...` for windows)
  - [ ] error display prefix (ie. `cmd /c hh.exe -800 ...`)
  - [ ] make sure the logfile does not exceed a certain size
- [ ] finalize documentation
  - [ ] document program execution steps
  - [ ] example reg file
  - [ ] documented ini file
  - [ ] complete list of error messages
  - [ ] README.txt
  - [ ] document limitations

NOTE: possible PCRE solution for mingw: https://nuwen.net/mingw.html

[1]: https://tools.ietf.org/html/rfc3986
[2]: https://www.gnu.org/software/libc/manual/html_node/Regular-Expressions.html#Regular-Expressions
[3]: https://www.gnu.org/software/sed/manual/html_node/Regular-Expressions.html
