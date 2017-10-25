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

The concatenated `authority` and `path` part make up the ini file section.
For example, `proto:aaa/bbb` would be mapped to the section `[aaa/bbb]`
in the ini file.

The `query` part is used to pass parameters to the executable. Special care
has to be taken when pasing in path information. 

## Ini file directives

### ~~exe~~
Absolute path to the executable. The path might use environment variables 
in the form of `${windir}\System32`. Case of the variable name *does* matter.

- Environment variable expansion: yes

### cmd
A template of the command to be executed. You must use absolute paths to files.
Environment variables in the form of ${env.VARNAME} will be expanded. URI query
parameters in the form of ${paramname} will also be epxanded.

- Environment variable expansion: yes

### default_path
All paths passed in as parameter must be within this base path. If a relative 
path is passed as parameter the base path will be treated as `cwd`. Relative 
paths may not start with `..`. A path which does not start with `[^A-Z:]`, 
`^/` and `^\.\.` is treated as relative path.

- Environment variable expansion: yes

### ~~params_transform~~
Mapping query parameter names to command line parameters. Passing a query 
string `?file=somefile.txt` might need to be converted into `/f somefile.txt`
on the command line. The syntax used in this directive is:

    urlparamname=cmdarg_name[ urlparamname=cmdarg_name [urlparamname=cmdarg_name]]

name value pairs are euqal seperated, pairs are space seperated.

- Environment variable expansion: no

### replace_file
This directive only works together with `replace_regex`. This directive
contains the file name on which the text needs to be replaced.

- Environment variable expansion: yes

### replace_regex
Defines a search and replace string as regular expression ([Regex][2]). The 
Format is written as used from sed, vi, etc. Example:
`/^(search)/replace $1/mi`.
See [Overview of Regular Expression Syntax][3] for more information on Posix regular expressions.

- Environment variable expansion: no

### ~~params_prepend~~
Add additional parameters to the command line before the parameters that are passed in via query.

### ~~params_append~~
Add additional parameters to the command line after the parameters that are passed in via query.

## Goals
- [x] open documents
- [x] run executables
  - [x] expand environment variables
  - [x] prepend parameters
  - [x] append static parameters
- [x] map url parmeter names to cmd parameter names
- [x] implement URI parser
- [x] implement ini parser
- [x] implement regex parser
  - [x] replace text in config files
  - [ ] implement regex modifiers
  - [ ] document regex syntax, capabilities, limitations
- [ ] sanitize and check paths
- [x] logging
  - [ ] configuable in ph.ini
- [x] checking if parameter passed username equals ${USERNAME}
- [x] display error messages if something goes south
- [ ] global config (log path, log level)
  - [ ] logging
  - [ ] global exe wrappers (ie. prefix `cmd /c ...` for windows)
  - [ ] error display prefix (ie. `cmd /c hh.exe -800 ...`)
  - [x] make sure the logfile does not exceed a certain size
- [ ] finalize documentation
  - [x] document program execution steps
  - [x] update example reg file
  - [ ] create documented ini file with example. compile it in
  - [ ] complete list of error messages, update error.html. compile error.html in.
  - [x] update README.txt, this is the program manual
  - [ ] update README.md, this is the build and programming manual
  - [ ] document limitations
- [x] implement unit tests

[1]: https://tools.ietf.org/html/rfc3986
[2]: https://www.gnu.org/software/libc/manual/html_node/Regular-Expressions.html#Regular-Expressions
[3]: https://www.gnu.org/software/sed/manual/html_node/Regular-Expressions.html
