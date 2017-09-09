Usage: PROGNAME_EXT <url>

SYNOPSYS:

This program implements a shell protocol handler. A shell might register this 
program (through registry on windows for example) and pass the called URI 
as parameter 1 to the program (input is also read from stdin).

The url is then translated a command line call (program and parameters). 
Programs need to be configured in PROGNAME.ini. This is a security measure 
so that only intended programs can be called.


USAGE:

On many modern platforms it is possible to register custom protocol handlers 
with the shell. This program is intended to map URIs to pre-confiured 
programms and execute certain actions as well as doing limited input validation 
for security reasons.

Once the program is called, it expects a URI as first parameter.
The URI is broken down into scheme, authority, path and query (see rfc3986,
section 3 for details). Once the described parts are extracted from the URI it 
is mapped to a local executable. The definition which URIs can be mapped to 
which executable is defined in PROGNAME.ini.

Due to the non-interactive nature of such a program, a debug log can be written 
to PROGNAME.log.

This a quick overview how url parts are mapped to an executable command line 
string (see rfc3986, section 3 for details on URIs):

  protohand://example/over/there?-a=1&-b=2#nose
        \_/   \______/\________/ \_______/ \__/
         |        |         |           |    |____
      scheme  authority     path        query   fragment
         |     ___|______   ____|____    __|____    |_
        / \   /          \ /         \  /       \     |
    [protocol]  [profile]  example.exe  -a=1 -b=2    unused

For this URI handler to work, it needs to be mapped to a protocol (foo in 
the above example URI). This is done trough registering the protocol name to 
the executable with your shell (in the registry for windows users).


CONFIGURATION:

You need to register this progam with you shell so that it recognizes your 
protocol name (Scheme) and redirects calls to thei executable. On Windows this 
is done trough the registry.

An example registry file would look like this. Replace <proto> with your 
protocol name (scheme) and <path> with an absolute path to your executable.

===[REG FILE]==================================================================
Windows Registry Editor Version 5.00

[HKEY_CLASSES_ROOT\<proto>]
@="URL: <proto> Protocol"
"URL Protocol"=""

[HKEY_CLASSES_ROOT\<proto>\shell]
[HKEY_CLASSES_ROOT\<proto>\shell\open]
[HKEY_CLASSES_ROOT\<proto>\shell\open\command]
@="\"C:\\<path>\\PROGNAME_EXT\" \"%1\""
===[EOF]=======================================================================

Mappings between URIs and executables are defined in PROGNAME.ini. 
If the ini file does not exist, it can be created by running the command once:
	c:\> PROGNAME_EXT [--create]

The authority part combined with the path are used to lookup a section in the 
ini file. The URI
  scheme://appname/exe/?-h
would match the following section in the ini file:
  [appname/exe]
   
Esample ini file:

===[INI FILE]==================================================================
;; NOTE: do not quote values

[exe/notepad]
; an example how to provide static command lin parameters, try it by running:
; protohand.exe "protohand://exe/notepad"
; result: 'c:\windows\notepad.exe /f c:\windows\OEMVer.txt'
exe = c:\windows\notepad.exe
params_prepend=/f c:\windows\OEMVer.txt

[exe/notepad/pathparam1]
; Example opening file from parameters, making suer the document is in a 
; certain directory. Files outside of `default_path` will not be opened:
; protohand.exe "protohand://exe/notepad/pathparam1?/f&c:\windows\OEMVer.txt"
; result: 'c:\windows\notepad.exe /f c:\windows\OEMVer.txt'
default_path=c:\windows\
exe = c:\windows\notepad.exe
allowed_params=/f
path_params=/f


[exe/notepad/pathparam2]
; Example that fails because the requested file is outside the allowed
; `default_path` folder:
; protohand.exe "protohand://exe/notepad/pathparam2?/A&c:\windows\OEMVer.txt"
; result: ERROR, will not open file because it is outside the base_path
default_path=\\some_server\some_share
allowed_params=/A,/U
path_params=/A
exe = c:\windows\notepad.exe
params_prepend=
params_append=

[exe/dfrgui]
; minimalistic config, just run a program
; protohand.exe "protohand://exe/dfrgui"
; FIXME: dfrgui.exe will not be launched, path is correct, permissions too 
; (at home, W10)
exe = C:\Windows\System32\dfrgui.exe

[exe/ie]
; launch internet explorer with an url provided in the query string:
; protohand.exe "protohand://exe/ie?http://www.google.com"
; result: '"C:\Program Files (x86)\Internet Explorer\iexplore.exe" 
;          "http://www.google.com"'
exe = C:\Program Files (x86)\Internet Explorer\iexplore.exe
===[EOF]=======================================================================

exe:            
     Path to the file which needs to be executed.

default_path (optional):   
     Base directory in which documents must reside. If the caller tries to 
     pass document paths outside of this directory, the program will abort 
     and not execute the exe.

path_params (optional):    
     A coma separated list parameters which contain file paths that need to 
     be checked.
  
alowed_params (optional):  
     A comma separated list of parameters which can be passed in by the 
     caller. This mechanism is white list based. If a caller passes parameters
     not listed here, the program mwill not execute the exe.

param_prepend (optiona):
	 Add this string in after the exe and before the arguments which were 
	 passed in the URIs query string. 

param_append (optiona):
	 Add this string at the endo of the command line. 


FILES:

	PROGNAME_EXT - This executable
	PROGNAME.ini - Mapping between urls and executables. This file needs
	               to be located in the same folder as the executable.
	PROGNAME.log - (optional) enabled on compile time with the flag 
	               -DDEBUG=1 The log will be written into the same 
	               directory as the executable. Make sure the program has 
	               write permission.


LIMITATIONS:

1. The maximung length of an URI is limited to STDIN_MAX bytes 
   (1024 by default).

2. The maximung length of a command (with all it's parameters) is limited 
   to MAX_CWD_LENGTH bytes (1024 by default).

3. the authority and path part in the URI are restricted to the following 
   characters as per rfc3986: [a-zA-Z0-9-._~] and in the path part 
   additionally '/' is allowed.

4. The length of the comma separated parameters list in the ini file is 
   limited to 100 items.

5. url parts are matched with the ini section case sesitive. if you call
   protohand://document/myapp but your section in the ini is called 
   [document/MyApp], the section will not be found.

6. Fully qualified path's to executables must be used in the the ini file's 
   `exe` directive. You might get lucky by using relative paths, but this can 
   be a security concern.

7. in any path that is passed in from query '.' and '..' are removed

8. It is unclear how this program will behave on multibyte character sets 
   coming from user input. It will probably crash and burn your computer and 
   desk down. Huzzah!


LICENSE:

This software is licensed by the BSD license. See LICENSE.txt in the source 
code for the full license text.

(c) 2017, Simon Wunderlin

Please send pull requests to https://github.com/wunderlins/protohand.git, any
fix and etension is appreciated!
