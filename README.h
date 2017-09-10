// Automatically generated file. Edit README.txt and run 
// 'make usage' to update this documentation!

char* usage_str = ""
"Usage: protohand.exe <url>\n"
"\n"
"SYNOPSYS:\n"
"\n"
"This program implements a shell protocol handler. A shell might register this \n"
"program (through registry on windows for example) and pass the called URI \n"
"as parameter 1 to the program (input is also read from stdin).\n"
"\n"
"The url is then translated a command line call (program and parameters). \n"
"Programs need to be configured in protohand.ini. This is a security measure \n"
"so that only intended programs can be called.\n"
"\n"
"\n"
"USAGE:\n"
"\n"
"On many modern platforms it is possible to register custom protocol handlers \n"
"with the shell. This program is intended to map URIs to pre-confiured \n"
"programms and execute certain actions as well as doing limited input validation \n"
"for security reasons.\n"
"\n"
"Once the program is called, it expects a URI as first parameter.\n"
"The URI is broken down into scheme, authority, path and query (see rfc3986,\n"
"section 3 for details). Once the described parts are extracted from the URI it \n"
"is mapped to a local executable. The definition which URIs can be mapped to \n"
"which executable is defined in protohand.ini.\n"
"\n"
"Due to the non-interactive nature of such a program, a debug log can be written \n"
"to protohand.log.\n"
"\n"
"This a quick overview how url parts are mapped to an executable command line \n"
"string (see rfc3986, section 3 for details on URIs):\n"
"\n"
"  protohand://example/over/there?-a=1&-b=2#nose\n"
"        \\_/   \\______/\\________/ \\_______/ \\__/\n"
"         |        |         |           |    |____\n"
"      scheme  authority     path        query   fragment\n"
"         |     ___|______   ____|____    __|____    |_\n"
"        / \\   /          \\ /         \\  /       \\     |\n"
"    [protocol]  [profile]  example.exe  -a=1 -b=2    unused\n"
"\n"
"For this URI handler to work, it needs to be mapped to a protocol (foo in \n"
"the above example URI). This is done trough registering the protocol name to \n"
"the executable with your shell (in the registry for windows users).\n"
"\n"
"\n"
"CONFIGURATION:\n"
"\n"
"You need to register this progam with you shell so that it recognizes your \n"
"protocol name (Scheme) and redirects calls to thei executable. On Windows this \n"
"is done trough the registry.\n"
"\n"
"An example registry file would look like this. Replace <proto> with your \n"
"protocol name (scheme) and <path> with an absolute path to your executable.\n"
"\n"
"===[REG FILE]==================================================================\n"
"Windows Registry Editor Version 5.00\n"
"\n"
"[HKEY_CLASSES_ROOT\\<proto>]\n"
"@=\"URL: <proto> Protocol\"\n"
"\"URL Protocol\"=\"\"\n"
"\n"
"[HKEY_CLASSES_ROOT\\<proto>\\shell]\n"
"[HKEY_CLASSES_ROOT\\<proto>\\shell\\open]\n"
"[HKEY_CLASSES_ROOT\\<proto>\\shell\\open\\command]\n"
"@=\"\\\"C:\\\\<path>\\\\protohand.exe\\\" \\\"%%1\\\"\"\n"
"\n"
";; internet explorer will show a warning when this protocol is used, you can \n"
";; disable this warning site wide or per user\n"
"; per user setting\n"
";[HKEY_CURRENT_USER\\Software\\Microsoft\\Internet Explorer\\ProtocolExecute\\<proto>]\n"
";\"WarnOnOpen\"=dword:00000000\n"
"\n"
"; computer setting\n"
"[HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Internet Explorer\\ProtocolExecute\\<proto>]\n"
"\"WarnOnOpen\"=dword:00000000\n"
"===[EOF]=======================================================================\n"
"\n"
"Mappings between URIs and executables are defined in protohand.ini. \n"
"If the ini file does not exist, it can be created by running the command once:\n"
"	c:\\> protohand.exe [--create]\n"
"\n"
"The authority part combined with the path are used to lookup a section in the \n"
"ini file. The URI\n"
"  scheme://appname/exe/?-h\n"
"would match the following section in the ini file:\n"
"  [appname/exe]\n"
"   \n"
"Esample ini file:\n"
"\n"
"===[INI FILE]==================================================================\n"
";; NOTE: do not quote values\n"
"\n"
"[exe/notepad]\n"
"; an example how to provide static command lin parameters, try it by running:\n"
"; protohand.exe \"protohand://exe/notepad\"\n"
"; result: 'c:\\windows\\notepad.exe /f c:\\windows\\OEMVer.txt'\n"
"exe = c:\\windows\\notepad.exe\n"
"params_prepend=/f c:\\windows\\OEMVer.txt\n"
"\n"
"[exe/notepad/pathparam1]\n"
"; Example opening file from parameters, making suer the document is in a \n"
"; certain directory. Files outside of `default_path` will not be opened:\n"
"; protohand.exe \"protohand://exe/notepad/pathparam1?/f&c:\\windows\\OEMVer.txt\"\n"
"; result: 'c:\\windows\\notepad.exe /f c:\\windows\\OEMVer.txt'\n"
"default_path=c:\\windows\\\n"
"exe = c:\\windows\\notepad.exe\n"
"allowed_params=/f\n"
"path_params=/f\n"
"\n"
"\n"
"[exe/notepad/pathparam2]\n"
"; Example that fails because the requested file is outside the allowed\n"
"; `default_path` folder:\n"
"; protohand.exe \"protohand://exe/notepad/pathparam2?/A&c:\\windows\\OEMVer.txt\"\n"
"; result: ERROR, will not open file because it is outside the base_path\n"
"default_path=\\\\some_server\\some_share\n"
"allowed_params=/A,/U\n"
"path_params=/A\n"
"exe = c:\\windows\\notepad.exe\n"
"params_prepend=\n"
"params_append=\n"
"\n"
"[exe/dfrgui]\n"
"; minimalistic config, just run a program\n"
"; protohand.exe \"protohand://exe/dfrgui\"\n"
"; FIXME: dfrgui.exe will not be launched, path is correct, permissions too \n"
"; (at home, W10)\n"
"exe = C:\\Windows\\System32\\dfrgui.exe\n"
"\n"
"[exe/ie]\n"
"; launch internet explorer with an url provided in the query string:\n"
"; protohand.exe \"protohand://exe/ie?http://www.google.com\"\n"
"; result: '\"C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe\" \n"
";          \"http://www.google.com\"'\n"
"exe = C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe\n"
"===[EOF]=======================================================================\n"
"\n"
"exe:            \n"
"     Path to the file which needs to be executed.\n"
"\n"
"default_path (optional):   \n"
"     Base directory in which documents must reside. If the caller tries to \n"
"     pass document paths outside of this directory, the program will abort \n"
"     and not execute the exe.\n"
"\n"
"path_params (optional):    \n"
"     A coma separated list parameters which contain file paths that need to \n"
"     be checked.\n"
"  \n"
"alowed_params (optional):  \n"
"     A comma separated list of parameters which can be passed in by the \n"
"     caller. This mechanism is white list based. If a caller passes parameters\n"
"     not listed here, the program mwill not execute the exe.\n"
"\n"
"param_prepend (optiona):\n"
"	 Add this string in after the exe and before the arguments which were \n"
"	 passed in the URIs query string. \n"
"\n"
"param_append (optiona):\n"
"	 Add this string at the endo of the command line. \n"
"\n"
"\n"
"FILES:\n"
"\n"
"	protohand.exe - This executable\n"
"	protohand.ini - Mapping between urls and executables. This file needs\n"
"	               to be located in the same folder as the executable.\n"
"	protohand.log - (optional) enabled on compile time with the flag \n"
"	               -DDEBUG=1 The log will be written into the same \n"
"	               directory as the executable. Make sure the program has \n"
"	               write permission.\n"
"\n"
"\n"
"LIMITATIONS:\n"
"\n"
"1. The maximung length of an URI is limited to 1024 bytes \n"
"   (1024 by default).\n"
"\n"
"2. The maximung length of a command (with all it's parameters) is limited \n"
"   to 1024 bytes (1024 by default).\n"
"\n"
"3. the authority and path part in the URI are restricted to the following \n"
"   characters as per rfc3986: [a-zA-Z0-9-._~] and in the path part \n"
"   additionally '/' is allowed.\n"
"\n"
"4. The length of the comma separated parameters list in the ini file is \n"
"   limited to 100 items.\n"
"\n"
"5. url parts are matched with the ini section case sesitive. if you call\n"
"   protohand://document/myapp but your section in the ini is called \n"
"   [document/MyApp], the section will not be found.\n"
"\n"
"6. Fully qualified path's to executables must be used in the the ini file's \n"
"   `exe` directive. You might get lucky by using relative paths, but this can \n"
"   be a security concern.\n"
"\n"
"7. in any path that is passed in from query '.' and '..' are removed\n"
"\n"
"8. It is unclear how this program will behave on multibyte character sets \n"
"   coming from user input. It will probably crash and burn your computer and \n"
"   desk down. Huzzah!\n"
"\n"
"\n"
"LICENSE:\n"
"\n"
"This software is licensed by the BSD license. See LICENSE.txt in the source \n"
"code for the full license text.\n"
"\n"
"(c) 2017, Simon Wunderlin\n"
"\n"
"Please send pull requests to https://github.com/wunderlins/protohand.git, any\n"
"fix and etension is appreciated!\n"
"";
