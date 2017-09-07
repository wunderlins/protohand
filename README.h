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
"programms and execute certain actions as well as limited input validation for\n"
"security reasons.\n"
"\n"
"Once the program is called, it expects a URI as first parameter or from stdin.\n"
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
"     foo://example/over/there?-a=1&-b=2#nose\n"
"     \\_/   \\______/\\________/ \\_______/ \\__/\n"
"      |        |         |           |    |____\n"
"   scheme  authority     path        query   fragment\n"
"      |     ___|______   ____|____    __|____    |_\n"
"     / \\   /          \\ /         \\  /       \\     |\n"
" [protocol]  [profile]  example.exe  -a=1 -b=2    unused\n"
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
"===[EOF]=======================================================================\n"
"\n"
"Mappings between URIs and executables are defined in protohand.ini. There are 2 \n"
"actions that can be configured:\n"
"1. Run an executable that expects 1 command line argument which points to a \n"
"   document. This method can check if the document path is within a certain \n"
"   location (for security reasons).\n"
"\n"
"2. Execut a program with any number of command line arguments. This method \n"
"   is only as secure as the configuration allows. You need to take care\n"
"   that no arbitrary programs or code can be run.\n"
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
"[word/document]\n"
"default_path=\"\\\\server\\share\"\n"
";allowed_params=\n"
";path_params=\n"
"exe = word.exe\n"
"\n"
"[cmd/exe]\n"
"exe = cmd.exe\n"
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
