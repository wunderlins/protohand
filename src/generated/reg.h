const char* reg_str = ""
"Windows Registry Editor Version 5.00\r\n"
";; Register a protocol handler\r\n"
";;\r\n"
";; REPLACE <proto> with your protocol name and <yourpath> with your path \r\n"
";; to the executable\r\n"
"\r\n"
"[HKEY_CLASSES_ROOT\\<proto>]\r\n"
"@=\"URL: <proto> Protocol\"\r\n"
"\"URL Protocol\"=\"\"\r\n"
"\r\n"
"[HKEY_CLASSES_ROOT\\<proto>\\shell]\r\n"
"[HKEY_CLASSES_ROOT\\<proto>\\shell\\open]\r\n"
"[HKEY_CLASSES_ROOT\\<proto>\\shell\\open\\command]\r\n"
"; replace <yourpath> with your installation location\r\n"
"@=\"\\\"<yourpath>\\\\ph.exe\\\" \\\"%%1\\\"\"\r\n"
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
"";
