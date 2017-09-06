Usage: PROGNAME <url>

SYNOPSYS
On many modern platforms it is possible to register custom protocol handlers 
with the shell. This program is intended to map urls to pre-confiured 
programms and execute certain actions as well as limited input validation for
security reasons.

Once the program is called, it expects a url as first parameter or from stdin.
The url is broken down into scheme, authority, path and query (see rfc3986,
section 3 for details).

USAGE

CONFIGURATION

FILES

LIMITATIONS

LICENSE
This software is licensed by the BSD license. See LICENSE.txt in the source 
code for the full license text.

(c) 2017, Simon Wunderlin
