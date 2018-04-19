// Automatically generated file. Edit example.ini and run 
// 'make ini' to update this documentation!
const char* ini_str = ""
";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\r\n"
";; this is the global configuration. you may use environment variables        ;;\r\n"
"[_global]\r\n"
"log_level = 5\r\n"
";log_path = ${env.APPDATA}\\ph.log\r\n"
";max_log_size_bytes = 10240000\r\n"
";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\r\n"
"\r\n"
"[ph.ini]\r\n"
"cmd = C:\\Windows\\notepad.exe /A \"${env.PH_HOME}\\ph.ini\"\r\n"
"\r\n"
"[userparam]\r\n"
"; ph.exe “ph://userparam?user=me“\r\n"
"user_param = user\r\n"
"; cmd  = this command will be ignored if user_param is present and the value is not empty\r\n"
"cmd_usermatch = cmd.exe\r\n"
"cmd_nousermatch = notepad.exe\r\n"
"\r\n"
"[test/fall]\r\n"
"; exmple how to trim and pad numbers with zero. lpadzero will left pad an url \r\n"
"; parameter with '0' up to a length of 9 characters. So the url parameter \r\n"
"; fall=1 will be represented on the command line as \"000000001\".\r\n"
"; ltrimzero does the opposite. An urlparameter pat=0000001 will be represented \r\n"
"; on the command line as \"1\".\r\n"
";\r\n"
"; ph.exe ph://test/fall?fall=1&pat=00000001\r\n"
"; result: \"testcmd.exe -fid 000000001 -pid 1\"\r\n"
"lpadzero = fall\r\n"
"ltrimzero = pat\r\n"
"cmd = ${env.PH_HOME}\\testcmd.exe -fid ${fall} -pid ${pat}\r\n"
"\r\n"
"[exe/testcmd]\r\n"
"; minimal example of running a test executable with no parameters from the uri\r\n"
";\r\n"
"; ph.exe \"ph://exe/testcmd\"\r\n"
"; result: 'C:\\Users\\wus\\Projects\\protohand\\testcmd.exe'\r\n"
"cmd = \"${env.PH_HOME}\\testcmd.exe\"\r\n"
"\r\n"
"[exe/notepad]\r\n"
"; an example how to provide static command line parameters, try it by running:\r\n"
";\r\n"
"; ph.exe \"ph://exe/notepad\"\r\n"
"; result: 'c:\\windows\\notepad.exe c:\\windows\\CSUP.txt'\r\n"
"cmd = \"${env.windir}\\notepad.exe\" \"c:\\windows\\CSUP.txt\"\r\n"
"\r\n"
"[exe/notepad2]\r\n"
"; an example how to execute an application and open a document.\r\n"
"; The filename is passed in by query string. notepad is started by cmd.exe\r\n"
"; but the cmd window will be hidden.\r\n"
";\r\n"
"; ph.exe \"ph://exe/notepad2?file=CSUP.txt\"\r\n"
"; result: 'c:\\windows\\notepad.exe c:\\windows\\CSUP.txt'\r\n"
"cmd = ${env.windir}\\system32\\cmd.exe /c \"${env.windir}\\notepad.exe\" \"c:\\windows\\${file}\"\r\n"
"start_hidden = 1\r\n"
"\r\n"
"\r\n"
"[exe/testcmd/configreplace]\r\n"
"; an example showing how to replace values in a config file before running \r\n"
"; the executable. The cfg_regreplace option takes a PCRE regular expression.\r\n"
";\r\n"
"; ph.exe \"ph://exe/testcmd/configreplace\"\r\n"
"; result: 'C:\\Users\\wus\\Projects\\protohand\\testcmd.exe'\r\n"
"cmd = ${env.PH_HOME}\\testcmd.exe\r\n"
"cfg_file = ${env.PH_HOME}\\testcfg.txt\r\n"
"cfg_regreplace = /(a)+/$1--/\r\n"
"\r\n"
"[exe/ie]\r\n"
"; launch internet explorer with an url provided in the query string:\r\n"
";\r\n"
"; ph.exe \"ph://exe/ie?url=http://www.google.com\"\r\n"
"; result: '\"C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe\" \"http://www.google.com\"'\r\n"
"cmd = \"C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe\" \"${url}\"\r\n"
"\r\n"
"[exe/dfrgui]\r\n"
"; minimalistic config, just run a program\r\n"
";\r\n"
"; ph.exe \"ph://exe/dfrgui\"\r\n"
"; FIXME: dfrgui.exe will not be launched, path is correct, permissions too (at home, W10)\r\n"
"cmd = C:\\Windows\\System32\\dfrgui.exe\r\n"
"\r\n"
"";
