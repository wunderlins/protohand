Usage: PROGNAME.exe <url>

SYNOPSYS

USAGE

On many modern platforms it is possible to register custom protocol handlers 
with the shell. This program is intended to map URIs to pre-confiured 
programms and execute certain actions as well as limited input validation for
security reasons.

Once the program is called, it expects a URI as first parameter or from stdin.
The URI is broken down into scheme, authority, path and query (see rfc3986,
section 3 for details). Once the described parts are extracted from the URI it 
is mapped to a local executable. The definition which URIs can be mapped to 
which executable is defined in PROGNAME.ini.

Due to the non-interactive nature of such a program, a debug log can be written 
to PROGNAME.log.

This a quick overview how url parts are mapped to an executable command line 
string:

     foo://example/over/there?-a=1&-b=2#nose
     \_/   \______/\________/ \_______/ \__/
      |        |         |           |    |__
   scheme  authority     path       query   fragment
      |     ___|______   ____|____    __|____    |_
     / \   /          \ /         \  /       \     |
 [protocol] example.exe  [profile]    -a=1 -b=2    unused

For this URI handler to work, it needs to be mapped to a protocol (foo in 
the above example URI). This is done trough registering the protocol name to 
the executable with your shell (in the registry for windows users).


CONFIGURATION

You need to register this progam with you shell so that it recognizes your 
protocol name (Scheme) and redirects calls to thei executable. On Windows this 
is done trough the registry.

An example registry file would look like this. Replace <proto> with your 
protocol name (scheme) and <path> with an absolute path to your executable.

===[REG FILE]===================================================================
Windows Registry Editor Version 5.00

[HKEY_CLASSES_ROOT\<proto>]
@="URL: <proto> Protocol"
"URL Protocol"=""

[HKEY_CLASSES_ROOT\<proto>\shell]
[HKEY_CLASSES_ROOT\<proto>\shell\open]
[HKEY_CLASSES_ROOT\<proto>\shell\open\command]
@="\"C:\\<path>\\protohand.exe\" \"%1\""
===[EOF]========================================================================

Mappings between URIs and executables are defined in PROGNAME.ini. There are 2 
actions that can be configured:
1. Run an executable that expects 1 command line argument which points to a 
   document. This method can check if the document path is within a certain 
   location (for security reasons).

2. Executa a program with any number of command line arguments. This method 
   is only as secure as the configuration allows. You need to take care
   that no arbitrary programs or code can be run.

Esample ini file:

===[INI FILE]===================================================================
[word/document]
default_path="\\server\share"
;allowed_params=
;path_params=
exe = word.exe

[cmd/exe]
exe = cmd.exe
===[EOF]========================================================================

exe:           TODO
allowed_path:  TODO
path_params:   TODO
default_path:  TODO


FILES

	PROGNAME.exe - This executable
	PROGNAME.ini - Mapping between urls and executables. This file needs to 
	               be located in the same folder as the executable.
	PROGNAME.log - (optional) enabled on compile time with the flag -DDEBUG=1
	               The log will be written into the same directory as the 
	               executable. Make sure the program has write permission.


LIMITATIONS
1. The maximung length of an URI is limited to STDIN_MAX bytes 
   (1024 by default).

2. The maximung length of a command (with all it's parameters) is limited 
   to MAX_CWD_LENGTH bytes (1024 by default).

LICENSE

This software is licensed by the BSD license. See LICENSE.txt in the source 
code for the full license text.

(c) 2017, Simon Wunderlin

Please send pull requests to https://github.com/wunderlins/protohand.git, any
fix and etension is appreciated!
