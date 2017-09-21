# protohand

minimal implementation of an rfc3986 uri handler. supported format:
scheme:[//]authority[/path[?query]]

The goal is to map an uri to a windows protocol hndler.
scheme:    handler (this exe, regiter with registry)
authority: mapped to exe file in ini file
path:      ??
query:     parameters for executable

NOTE: The current implementation reads URIs from stdin. might have to change 
this for the windows protocol handler, since itm ight pass it in via argv[1].

LIMITATIONS
- max 1024 chars length of URIs
- only scheme:authority:path:query parts parsed


## environment
- install mingw from https://nuwen.net/files/mingw/history/mingw-15.1.exe
- edit `c:\<installdir>\open_distro_paths.bat` and add the following line after `doskey`: `start git\git-bash.exe && exit`
- add the following line to `c:\<installdir>\git\etc\fstab`: `c:\<installdir> /mingw` 
- start the build environment by running `open_distro_window.bat`
- cd to your desired project directory and chout the source: `git clone git@github.com:wunderlins/protohand.git`
- set proper crlf handling `git config --global core.autocrlf false`

## build

	make all
	make test

ini parser from https://github.com/benhoyt/inih (BSD license)


## referemces

Registering an Application to a URI Scheme:
https://msdn.microsoft.com/en-us/library/aa767914(v=vs.85).aspx

About Asynchronous Pluggable Protocols
https://msdn.microsoft.com/en-us/library/aa767916(v=vs.85).aspx

A primer for windows:
https://blogs.msdn.microsoft.com/ieinternals/2011/07/13/understanding-protocols/