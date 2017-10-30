# protohand - a protocol handler

minimalistic implementation of an rfc3986 uri handler. supported format:
`scheme:[//]authority[/path[?query]]`

The program will map an uri to a windows protocol handler. The `authority` and 
`path` are then matched against an ini file section in the configuration file 
which is called `[authority/path]`. This ini section contains instructions how 
to run a program.

In short, this program makes it possible to launch many external programs 
trough one mechanism. It is able to pass parameters to the called program in a 
way it would expect it.

	scheme:    handler (this exe, regiter with registry)
	authority: mapped to exe file in ini file
	path:      optional additional mapping options to authority
	query:     parameters for executable

## Documentation

- [README](doc/README.txt)
- [INSTALL](doc/INSTALL.txt)
- [BUILD](doc/BUILD.txt)
- [LICENSE](doc/LICENSE.txt)
- if you want to call custom protocol handlers from your application (w32):
  - [C++](doc/integration/cplusplus.cpp)
  - [C#](doc/integration/csharp.cs)
  - [PowerShell](doc/integration/powershell.ps1)
  - [CMD](doc/integration/batch.cmd)
  - [HTML](doc/integration/html.html)

## Credits

- ini parser from https://github.com/benhoyt/inih (BSD license)
- Regular expression support is provided by the PCRE library package http://pcre.org/


## References

Registering an Application to a URI Scheme:
https://msdn.microsoft.com/en-us/library/aa767914(v=vs.85).aspx

About Asynchronous Pluggable Protocols
https://msdn.microsoft.com/en-us/library/aa767916(v=vs.85).aspx

A primer for windows:
https://blogs.msdn.microsoft.com/ieinternals/2011/07/13/understanding-protocols/

PCRE (Perl Compatible Regular Expressions):
https://www.rexegg.com/pcre-doc/_latest/pcresyntax.html

Cross-Platform: https://support.shotgunsoftware.com/hc/en-us/articles/219031308-Launching-applications-using-custom-browser-protocols