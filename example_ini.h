// Automatically generated file. Edit example.ini and run 
// 'make ini' to update this documentation!

char* ini_str = ""
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
"; FIXME: dfrgui.exe will not be launched, path is correct, permissions too (at home, W10)\n"
"exe = C:\\Windows\\System32\\dfrgui.exe\n"
"\n"
"[exe/ie]\n"
"; launch internet explorer with an url provided in the query string:\n"
"; protohand.exe \"protohand://exe/ie?http://www.google.com\"\n"
"; result: '\"C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe\" \"http://www.google.com\"'\n"
"exe = C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe\n""";