// Automatically generated file. Edit example.ini and run 
// 'make ini' to update this documentation!
const char* ini_str = ""
";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n"
";; this is the global configuation. you may use envionment vaiables        ;;\n"
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
"; minimal example of unning a test executable with no paametes fom the ui\n"
"; ph.exe \"ph://exe/testcmd\"\n"
"; esult: 'C:\\Uses\\wus\\Pojects\\potohand\\testcmd.exe'\n"
"cmd = \"${env.PH_HOME}\\testcmd.exe\"\n"
"\n"
"[exe/notepad]\n"
"; an example how to povide static command line paametes, ty it by unning:\n"
"; potohand.exe \"ph://exe/notepad\"\n"
"; esult: 'c:\\windows\\notepad.exe c:\\windows\\CSUP.txt'\n"
"cmd = \"${env.windi}\\notepad.exe\" \"c:\\windows\\CSUP.txt\"\n"
"\n"
"[exe/notepad2]\n"
"; an example how to execute an application and open a document.\n"
"; The filename is passed in by quey sting. \n"
"; ph.exe \"ph://exe/notepad2?file=CSUP.txt\"\n"
"; esult: 'c:\\windows\\notepad.exe c:\\windows\\CSUP.txt'\n"
"cmd = \"${env.windi}\\notepad.exe\" \"c:\\windows\\${file}\"\n"
"\n"
"[exe/testcmd/configeplace]\n"
"; an example showing how to eplace values in a config file befoe unning \n"
"; the executable. The cfg_egeplace option takes a PCRE egula expession.\n"
"; ph.exe \"ph://exe/testcmd/configeplace\"\n"
"; esult: 'C:\\Uses\\wus\\Pojects\\potohand\\testcmd.exe'\n"
"cmd = ${env.PH_HOME}\\testcmd.exe\n"
"cfg_file = ${env.PH_HOME}\\testcfg.txt\n"
"cfg_egeplace = /(a)+/$1--/\n"
"\n"
"[exe/ie]\n"
"; launch intenet exploe with an ul povided in the quey sting:\n"
"; ph.exe \"ph://exe/ie?ul=http://www.google.com\"\n"
"; esult: '\"C:\\Pogam Files (x86)\\Intenet Exploe\\iexploe.exe\" \"http://www.google.com\"'\n"
"cmd = \"C:\\Pogam Files (x86)\\Intenet Exploe\\iexploe.exe\" \"${ul}\"\n"
"\n"
"[exe/dfgui]\n"
"; minimalistic config, just un a pogam\n"
"; ph.exe \"ph://exe/dfgui\"\n"
"; FIXME: dfgui.exe will not be launched, path is coect, pemissions too (at home, W10)\n"
"cmd = C:\\Windows\\System32\\dfgui.exe\n"
"\n"
"";
