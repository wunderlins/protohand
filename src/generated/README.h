// Automatically generated file. Edit README.txt and run 
// 'make usage' to update this documentation!
const char* usage_str = ""
"NAME\n"
"\n"
"    ph - a shell protocol handler to run external programs\n"
"\n"
"\n"
"SYNOPSIS\n"
"\n"
"    ph <url> [config_file]\n"
"    ph -e <config_file> [out_file]\n"
"    ph -r </regex/replace/> <file>\n"
"    ph -h \n"
"\n"
"\n"
"DESCRIPTION\n"
"\n"
"    ph can be registered as protocol handler by shells that support \n"
"    global custom protocol handlers. The program will expect a complete url as \n"
"    first parameter. The authority and path parameter parts of the url are then\n"
"    used to look up a section in the configuration file which holds \n"
"    instructions what needs to be executed. The config file is written in the \n"
"    form of an ini file or it can be compiled into an encoded file to hide \n"
"    sensitive information such as passwords.\n"
"    \n"
"    If the optional [config_file] parameter is provided, the program will \n"
"    load the configuration from this location. This might be a UNC path, \n"
"    a local file or a remote file from an http/ftp server. If parameter 2 is \n"
"    ommited, the programm will first look for a `ph.dat` and then \n"
"    for `ph.ini` in the same folder as the executable. See \n"
"    CONFIGURATION for mor information.\n"
"    \n"
"    Once the program is registered to handle a specific protocol scheme, it must\n"
"    be called by the shell with the full url as first parameter. As an example\n"
"    (assuming the program is registered with the protocol scheme `proto:`):\n"
"    \n"
"        proto://authority/path/dir?param1=value1\n"
"    \n"
"    can be mapped to run the following program:\n"
"        \n"
"        some_executable.exe --file=\"${param1}\"\n"
"    \n"
"    where `${param1}` will b substitued by the value of the query parameter \n"
"    `value1`. Parameter names are case insensitive. \n"
"    \n"
"    The mapping is done throug the `ph.ini` file. Detailed \n"
"    information on how to use the configuration file can be found in \n"
"    INSTALLATION section.\n"
"    \n"
"    But in short, the following configuration would be needed to make the above\n"
"    example work:\n"
"    \n"
"        [authority/path/dir] ; this is the mapping to the url\n"
"        cmd = some_executable.exe --file=\"${param1}\"\n"
"\n"
"    Registering a protocol scheme can be done trough a registry setting on \n"
"    windows. An example registrz file should come with this program if not \n"
"    run it in a folder without a configuration file and it will be created.\n"
"\n"
"\n"
"OPTIONS\n"
"\n"
"    -e encode a plain text ini config file. the resulting file will be stored\n"
"       in the same folder bit with a file extension of `.dat`. Mone information\n"
"       the usage of this option can be found in INSTALL.txt\n"
"    \n"
"    -r /regex/replace/ will test the build in regex replace mechanism. Mone \n"
"       information the usage of this option can be found in INSTALL.txt\n"
"\n"
"    -h This help documentation.\n"
"\n"
"INSTALLATION\n"
"\n"
"    The minimum installation requires the `ph` being placed in \n"
"    your desired installation directory. If the program is run once, it will \n"
"    produce an example `ph.ini` file in the same folder which is \n"
"    the configuration file. It will also create a `ph.log` file \n"
"    for debugging and a `ph.reg` for registration as windows \n"
"    protocol handler.\n"
"\n"
"\n"
"WINDOWS REGISTRY\n"
"\n"
"    The `ph.reg` file includes two place holders `<proto>` with your\n"
"    desired protocol name. If, say, your shell shall route all calls to \n"
"    `myprotocol://someuri` to `ph`, then replace `<proto>` with\n"
"    `myprotocol`. You will need to replace `<yourpath>` with the path to the \n"
"    `ph` executable. Make sure to use double backslashes `\\\\` in\n"
"    the path, a single backslash is interpreted as escape character.\n"
"\n"
"    The example registry file includes 2 examples how to suppress the warning in\n"
"    internet explorer. One for the user registry (per user), one for the local \n"
"    machine (system wide).\n"
"\n"
"\n"
"CONFIGURATION\n"
"\n"
"    The program will first check if it is called with a 2nd parameter. If so, \n"
"    it will search the configuration file in this location. This might be a \n"
"    local file, an UNC path rl an http/ftp url. If parameter 2 is not provided, \n"
"    `ph` will look in the isntallation directory for a file \n"
"    called `ph.ini` or `ph.dat`. The latter is an \n"
"    obfuscated file which hides sensitive data such as passwords.\n"
"\n"
"    You can create an obfuscated file by running:\n"
"\n"
"        ph -e <config_file> [out_file]\n"
"\n"
"    if parameter 2 is omitted, the obfuscated file will be stored in the same \n"
"    directory as the source file but with a `.dat` ending.\n"
"\n"
"    The configuration file is in INI format and contains sections and \n"
"    name=value pairs. The sectiosn are enclosed in brackets and represent the \n"
"    authority and path part of an URI. If you, for example, run the following \n"
"    url `proto://something/else` through `ph`, the the program \n"
"    will look for the following section in the ini file: \n"
"    [something/else]. If found, all name value pairs are read and interpreted.\n"
"\n"
"Configuration directives:\n"
"\n"
"    cmd: \n"
"        The command to execute. This is the main config value which does\n"
"        define the full command line. Make sure to use absolute paths.\n"
"        \n"
"        Variable substitution is available for environment variables\n"
"        and URI query parameters. The URI query parameters are represented \n"
"        in the following format in the cmd value: `${param_name}` which will\n"
"        corespond to the query parameter `?param_name=value` and be expanded\n"
"        to `value`. If the query variable cannot be found, the proram will \n"
"        abort.\n"
"        \n"
"        Environment variables can be inserted in the cmd configuration in the \n"
"        following format: `${env.APPDATA}`, where `APPDATA` must be a valid \n"
"        environment variable. If it cannot be found, the program will abort.\n"
"    \n"
"    replace_regex: \n"
"        It is possible to run a regular expression search&replace against any \n"
"        file on this system. Regular expression are deifind in the following \n"
"        form /regex/replace/. Back references cane be used in the replace \n"
"        syntax and must be prepended by `$` (eg. `$1`).\n"
"        \n"
"        This program uses the Perl Compatible Regular Expresseion (PCRE) \n"
"        syntax. More on PCRE here: \n"
"        https://www.rexegg.com/pcre-doc/_latest/pcresyntax.html\n"
"    \n"
"    replace_file: \n"
"        File to run a regex against\n"
"    \n"
"\n"
"ENVIRONMENT\n"
"\n"
"    PH_HOME  is set upon startup of the program in it's own process and will \n"
"             point to the folder of the executable. This variable can be used \n"
"             in the configuration file to point to the application directory.\n"
"\n"
"\n"
"FILES\n"
"\n"
"    $PH_HOME/ph.ini      The default location of the configration file. If the \n"
"                         2nd argument is not provided, `ph`\n"
"                         will search for the configuration file here.\n"
"\n"
"    $PH_HOME/ph.dat      The default location of the encoded configration file.\n"
"                         To encode an `.ini` file into a `.dat` file see \n"
"                         the `-e` command line switch.\n"
"\n"
"    ph.log               Logfile of the program. The location of the logfile is\n"
"                         detiremined by two environment variables. if the \n"
"                         variable APPDATA can be resolved, it will be put there\n"
"                         as `ph.log` otherwise the program checks\n"
"                         for `HOME` if found, the log file will be stored \n"
"                         as `.ph.log`. If none of the two \n"
"                         environment variables is available we panic an \n"
"                         abort :-/\n"
"\n"
"    $PH_HOME/error.html  Error messages for display in a browser when the \n"
"                         program encounters a problem.\n"
"\n"
"\n"
"LIMITATIONS\n"
"\n"
"    1. The maximung length of a command (with all it's parameters) is limited \n"
"       to 4096 bytes.\n"
"\n"
"    2. the authority and path part in the URI are restricted to the following \n"
"       characters as per rfc3986: [a-zA-Z0-9-._~] and in the path part \n"
"       additionally '/' is allowed. This programm will parse any value but the \n"
"       shell might handle any other character in an unpredictable way, so avoid\n"
"       them in ini sections.\n"
"\n"
"    3. Fully qualified path's to executables must be used in the the ini file's \n"
"       `exe` directive. You might get lucky by using relative paths, but this \n"
"       can be a security concern.\n"
"\n"
"    4. It is unclear how this program will behave on multibyte character sets \n"
"       coming from user input. It will probably crash and burn your computer \n"
"       and desk down. Huzzah!\n"
"\n"
"\n"
"COPYRIGHT\n"
"\n"
"    2017, Simon Wunderlin (BSD license)\n"
"    2009, Ben Hoyt - Ini parser, https://github.com/benhoyt/inih (BSD license)\n"
"    1996 - 2017 Daniel Stenberg, daniel@haxx.se - cURL (BSD license)\n"
"    \n"
"    Regular expression support is provided by the PCRE library package,\n"
"    which is open source software, written by Philip Hazel, and copyright\n"
"    by the University of Cambridge, England.\n"
"\n"
"    Copyright OpenSSL 2017\n"
"    Contents licensed under the terms of the OpenSSL license\n"
"    See http://www.openssl.org/source/license.html for details\n"
"\n"
"SEE ALSO\n"
"\n"
"    INSTALL.txt        for installation and configuration instructions.\n"
"    BUILD.txt          for instructions on building for Windows, OSX and Linux\n"
"    LICENSE.txt        BSD License\n"
"    ph.reg             Example registry file\n"
"    ph.ini             Configuration file\n"
"    ph.dat             Encoded configuration file\n"
"";
