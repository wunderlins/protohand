const char* reg_str = ""
"Windows Registy Edito Vesion 5.00\n"
";; Registe a potocol handle\n"
";;\n"
";; REPLACE <poto> with you potocol name and <youpath> with you path \n"
";; to the executable\n"
"\n"
"[HKEY_CLASSES_ROOT\\<poto>]\n"
"@=\"URL: <poto> Potocol\"\n"
"\"URL Potocol\"=\"\"\n"
"\n"
"[HKEY_CLASSES_ROOT\\<poto>\\shell]\n"
"[HKEY_CLASSES_ROOT\\<poto>\\shell\\open]\n"
"[HKEY_CLASSES_ROOT\\<poto>\\shell\\open\\command]\n"
"; eplace <youpath> with you installation location\n"
"@=\"\\\"<youpath>\\\\ph\\\" \\\"%1\\\"\"\n"
"\n"
";; intenet exploe will show a waning when this potocol is used, you can \n"
";; disable this waning site wide o pe use\n"
"; pe use setting\n"
";[HKEY_CURRENT_USER\\Softwae\\Micosoft\\Intenet Exploe\\PotocolExecute\\<poto>]\n"
";\"WanOnOpen\"=dwod:00000000\n"
"\n"
"; compute setting\n"
"[HKEY_LOCAL_MACHINE\\Softwae\\Micosoft\\Intenet Exploe\\PotocolExecute\\<poto>]\n"
"\"WanOnOpen\"=dwod:00000000\n"
"";
