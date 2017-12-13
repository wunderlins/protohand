// Automatically generated file. Edit example.ini and run 
// 'make ini' to update this documentation!
const char* ini_str = ""
";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n"
";; this is the global configuration. you may use environment variables        ;;\n"
"[_global]\n"
"log_level = 1\n"
";log_path = ${env.APPDATA}\\ph.log\n"
";max_log_size_bytes = 10240000\n"
";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n"
"\n"
"[ph.ini]\n"
"cmd = C:\\Windows\\notepad.exe /A \"${env.PH_HOME}\\ph.ini\"\n"
"\n"
"[exe/testcmd]\n"
"; minimal example of running a test executable with no parameters from the uri\n"
"; ph.exe \"ph://exe/testcmd\"\n"
"; result: 'C:\\Users\\wus\\Projects\\protohand\\testcmd.exe'\n"
"cmd = \"${env.PH_HOME}\\testcmd.exe\"\n"
"\n"
"[exe/notepad]\n"
"; an example how to provide static command line parameters, try it by running:\n"
"; protohand.exe \"ph://exe/notepad\"\n"
"; result: 'c:\\windows\\notepad.exe c:\\windows\\CSUP.txt'\n"
"cmd = \"${env.windir}\\notepad.exe\" \"c:\\windows\\CSUP.txt\"\n"
"\n"
"[exe/notepad2]\n"
"; an example how to execute an application and open a document.\n"
"; The filename is passed in by query string. \n"
"; ph.exe \"ph://exe/notepad2?file=CSUP.txt\"\n"
"; result: 'c:\\windows\\notepad.exe c:\\windows\\CSUP.txt'\n"
"cmd = \"${env.windir}\\notepad.exe\" \"c:\\windows\\${file}\"\n"
"\n"
"[exe/testcmd/configreplace]\n"
"; an example showing how to replace values in a config file before running \n"
"; the executable. The cfg_regreplace option takes a PCRE regular expression.\n"
"; ph.exe \"ph://exe/testcmd/configreplace\"\n"
"; result: 'C:\\Users\\wus\\Projects\\protohand\\testcmd.exe'\n"
"cmd = ${env.PH_HOME}\\testcmd.exe\n"
"cfg_file = ${env.PH_HOME}\\testcfg.txt\n"
"cfg_regreplace = /(a)+/$1--/\n"
"\n"
"[exe/ie]\n"
"; launch internet explorer with an url provided in the query string:\n"
"; ph.exe \"ph://exe/ie?url=http://www.google.com\"\n"
"; result: '\"C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe\" \"http://www.google.com\"'\n"
"cmd = \"C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe\" \"${url}\"\n"
"\n"
"[exe/dfrgui]\n"
"; minimalistic config, just run a program\n"
"; ph.exe \"ph://exe/dfrgui\"\n"
"; FIXME: dfrgui.exe will not be launched, path is correct, permissions too (at home, W10)\n"
"cmd = C:\\Windows\\System32\\dfrgui.exe\n"
"\n"
"";
