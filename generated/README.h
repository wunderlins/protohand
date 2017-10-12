// Automatically generated file. Edit README.txt and run 
// 'make usage' to update this documentation!

const char* usage_str = ""
"Usage: protohand.exe <url>\r\n"
"\r\n"
"SYNOPSYS:\r\n"
"\r\n"
"This program implements a shell protocol handler. A shell might register this \r\n"
"program (through registry on windows for example) and pass the called URI \r\n"
"as parameter 1 to the program (input is also read from stdin).\r\n"
"\r\n"
"The url is then translated a command line call (program and parameters). \r\n"
"Programs need to be configured in protohand.ini. This is a security measure \r\n"
"so that only intended programs can be called.\r\n"
"\r\n"
"\r\n"
"USAGE:\r\n"
"\r\n"
"On many modern platforms it is possible to register custom protocol handlers \r\n"
"with the shell. This program is intended to map URIs to pre-confiured \r\n"
"programms and execute certain actions as well as doing limited input validation \r\n"
"for security reasons.\r\n"
"\r\n"
"Once the program is called, it expects a URI as first parameter.\r\n"
"The URI is broken down into scheme, authority, path and query (see rfc3986,\r\n"
"section 3 for details). Once the described parts are extracted from the URI it \r\n"
"is mapped to a local executable. The definition which URIs can be mapped to \r\n"
"which executable is defined in protohand.ini.\r\n"
"\r\n"
"Due to the non-interactive nature of such a program, a debug log can be written \r\n"
"to protohand.log.\r\n"
"\r\n"
"This a quick overview how url parts are mapped to an executable command line \r\n"
"string (see rfc3986, section 3 for details on URIs):\r\n"
"\r\n"
"         ph://example/over/there?-a=1&-b=2#nose\r\n"
"        \\_/   \\______/\\________/ \\_______/ \\__/\r\n"
"         |        |         |           |    |____\r\n"
"      scheme  authority     path        query   fragment\r\n"
"         |     ___|____________|____    __|____    |_\r\n"
"        / \\   /                     \\  /       \\     |\r\n"
"    [protocol]      example.exe        -a=1 -b=2    unused\r\n"
"\r\n"
"For this URI handler to work, it needs to be mapped to a protocol (foo in \r\n"
"the above example URI). This is done trough registering the protocol name to \r\n"
"the executable with your shell (in the registry for windows users).\r\n"
"\r\n"
"\r\n"
"PASSING PARAMETERS:\r\n"
"\r\n"
"The query part of the URI will be converted into command line arguments. First\r\n"
"parameter names will be translated to their command line argument equivalents \r\n"
"if the ini file derictive contains the parameter 'params_transform'. Typically\r\n"
"command line arguments can be just one value or a name=value pair. This is also\r\n"
"true for query parameters. Command line arguments are sperated by whitespace while \r\n"
"query arguments are spearated by '&'. If you wish to pass parameters with \r\n"
"values, then use the following query string notation:\r\n"
"    ?name1=value1&name2=value2\r\n"
"This woud be translated on the command line to:\r\n"
"    name1=value1 name2=value2\r\n"
"\r\n"
"If you wish to juts pass in values (without a name) then use the follwing URI \r\n"
"query syntax:\r\n"
"    ?value1&value2\r\n"
"which will translate on the command-line to the following parameters:\r\n"
"    value1 value2\r\n"
"\r\n"
"\r\n"
"CONFIGURATION:\r\n"
"\r\n"
"You need to register this progam with you shell so that it recognizes your \r\n"
"protocol name (Scheme) and redirects calls to thei executable. On Windows this \r\n"
"is done trough the registry.\r\n"
"\r\n"
"An example registry file would look like this. Replace <proto> with your \r\n"
"protocol name (scheme) and <path> with an absolute path to your executable.\r\n"
"\r\n"
"===[REG FILE]==================================================================\r\n"
"Windows Registry Editor Version 5.00\r\n"
"\r\n"
"[HKEY_CLASSES_ROOT\\<proto>]\r\n"
"@=\"URL: <proto> Protocol\"\r\n"
"\"URL Protocol\"=\"\"\r\n"
"\r\n"
"[HKEY_CLASSES_ROOT\\<proto>\\shell]\r\n"
"[HKEY_CLASSES_ROOT\\<proto>\\shell\\open]\r\n"
"[HKEY_CLASSES_ROOT\\<proto>\\shell\\open\\command]\r\n"
"@=\"\\\"C:\\\\<path>\\\\protohand.exe\\\" \\\"%%1\\\"\"\r\n"
"\r\n"
";; internet explorer will show a warning when this protocol is used, you can \r\n"
";; disable this warning site wide or per user\r\n"
"; per user setting\r\n"
";[HKEY_CURRENT_USER\\Software\\Microsoft\\Internet Explorer\\ProtocolExecute\\<proto>]\r\n"
";\"WarnOnOpen\"=dword:00000000\r\n"
"\r\n"
"; computer setting\r\n"
"[HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Internet Explorer\\ProtocolExecute\\<proto>]\r\n"
"\"WarnOnOpen\"=dword:00000000\r\n"
"===[EOF]=======================================================================\r\n"
"\r\n"
"Mappings between URIs and executables are defined in protohand.ini. \r\n"
"If the ini file does not exist, it can be created by running the command once:\r\n"
"	c:\\> protohand.exe [--create]\r\n"
"\r\n"
"The authority part combined with the path are used to lookup a section in the \r\n"
"ini file. The URI\r\n"
"  scheme://appname/exe/?-h\r\n"
"would match the following section in the ini file:\r\n"
"  [appname/exe]\r\n"
"   \r\n"
"Esample ini file:\r\n"
"\r\n"
"===[INI FILE]==================================================================\r\n"
";; NOTE: do not quote values\r\n"
"\r\n"
"[exe/notepad]\r\n"
"; an example how to provide static command lin parameters, try it by running:\r\n"
"; protohand.exe \"protohand://exe/notepad\"\r\n"
"; result: 'c:\\windows\\notepad.exe /f c:\\windows\\OEMVer.txt'\r\n"
"exe = c:\\windows\\notepad.exe\r\n"
"params_prepend=/f c:\\windows\\OEMVer.txt\r\n"
"\r\n"
"[exe/notepad/pathparam1]\r\n"
"; Example opening file from parameters, making suer the document is in a \r\n"
"; certain directory. Files outside of `default_path` will not be opened:\r\n"
"; protohand.exe \"protohand://exe/notepad/pathparam1?/f&c:\\windows\\OEMVer.txt\"\r\n"
"; result: 'c:\\windows\\notepad.exe /f c:\\windows\\OEMVer.txt'\r\n"
"default_path=c:\\windows\\\r\n"
"exe = c:\\windows\\notepad.exe\r\n"
"allowed_params=/f\r\n"
"path_params=/f\r\n"
"\r\n"
"\r\n"
"[exe/notepad/pathparam2]\r\n"
"; Example that fails because the requested file is outside the allowed\r\n"
"; `default_path` folder:\r\n"
"; protohand.exe \"protohand://exe/notepad/pathparam2?/A&c:\\windows\\OEMVer.txt\"\r\n"
"; result: ERROR, will not open file because it is outside the base_path\r\n"
"default_path=\\\\some_server\\some_share\r\n"
"allowed_params=/A,/U\r\n"
"path_params=/A\r\n"
"exe = c:\\windows\\notepad.exe\r\n"
"params_prepend=\r\n"
"params_append=\r\n"
"\r\n"
"[exe/dfrgui]\r\n"
"; minimalistic config, just run a program\r\n"
"; protohand.exe \"protohand://exe/dfrgui\"\r\n"
"; FIXME: dfrgui.exe will not be launched, path is correct, permissions too \r\n"
"; (at home, W10)\r\n"
"exe = C:\\Windows\\System32\\dfrgui.exe\r\n"
"\r\n"
"[exe/ie]\r\n"
"; launch internet explorer with an url provided in the query string:\r\n"
"; protohand.exe \"protohand://exe/ie?http://www.google.com\"\r\n"
"; result: '\"C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe\" \r\n"
";          \"http://www.google.com\"'\r\n"
"exe = C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe\r\n"
"===[EOF]=======================================================================\r\n"
"\r\n"
"exe:            \r\n"
"     Path to the file which needs to be executed.\r\n"
"\r\n"
"default_path (optional):   \r\n"
"     Base directory in which documents must reside. If the caller tries to \r\n"
"     pass document paths outside of this directory, the program will abort \r\n"
"     and not execute the exe.\r\n"
"\r\n"
"path_params (optional):    \r\n"
"     A coma separated list parameters which contain file paths that need to \r\n"
"     be checked.\r\n"
"  \r\n"
"alowed_params (optional):  \r\n"
"     A comma separated list of parameters which can be passed in by the \r\n"
"     caller. This mechanism is white list based. If a caller passes parameters\r\n"
"     not listed here, the program mwill not execute the exe.\r\n"
"\r\n"
"param_prepend (optiona):\r\n"
"	 Add this string in after the exe and before the arguments which were \r\n"
"	 passed in the URIs query string. \r\n"
"\r\n"
"param_append (optiona):\r\n"
"	 Add this string at the endo of the command line. \r\n"
"\r\n"
"\r\n"
"FILES:\r\n"
"\r\n"
"	protohand.exe - This executable\r\n"
"	protohand.ini - Mapping between urls and executables. This file needs\r\n"
"	               to be located in the same folder as the executable.\r\n"
"	protohand.log - (optional) enabled on compile time with the flag \r\n"
"	               -DDEBUG=1 The log will be written into the same \r\n"
"	               directory as the executable. Make sure the program has \r\n"
"	               write permission.\r\n"
"\r\n"
"\r\n"
"LIMITATIONS:\r\n"
"\r\n"
"1. The maximung length of an URI is limited to 1024 bytes \r\n"
"   (1024 by default).\r\n"
"\r\n"
"2. The maximung length of a command (with all it's parameters) is limited \r\n"
"   to 260 bytes (1024 by default).\r\n"
"\r\n"
"3. the authority and path part in the URI are restricted to the following \r\n"
"   characters as per rfc3986: [a-zA-Z0-9-._~] and in the path part \r\n"
"   additionally '/' is allowed.\r\n"
"\r\n"
"4. The length of the comma separated parameters list in the ini file is \r\n"
"   limited to 100 items.\r\n"
"\r\n"
"5. url parts are matched with the ini section case sesitive. if you call\r\n"
"   protohand://document/myapp but your section in the ini is called \r\n"
"   [document/MyApp], the section will not be found.\r\n"
"\r\n"
"6. Fully qualified path's to executables must be used in the the ini file's \r\n"
"   `exe` directive. You might get lucky by using relative paths, but this can \r\n"
"   be a security concern.\r\n"
"\r\n"
"7. in any path that is passed in from query '.' and '..' are removed\r\n"
"\r\n"
"8. It is unclear how this program will behave on multibyte character sets \r\n"
"   coming from user input. It will probably crash and burn your computer and \r\n"
"   desk down. Huzzah!\r\n"
"\r\n"
"\r\n"
"LICENSE:\r\n"
"\r\n"
"This software is licensed by the BSD license. See LICENSE.txt in the source \r\n"
"code for the full license text.\r\n"
"\r\n"
"(c) 2017, Simon Wunderlin\r\n"
"\r\n"
"Please send pull requests to https://github.com/wunderlins/protohand.git, any\r\n"
"fix and extension is appreciated!\r\n"
"";
