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
- use mingw installer, the following modules are required
	- mingw-developer-toolkit
	- mingw32-base (gcc)
	- msys-base (shell)

## build

	make all
	make test

ini parser from https://github.com/benhoyt/inih (BSD license)
