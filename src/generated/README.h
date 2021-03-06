// Automatically generated file. Edit README.txt and run 
// 'make usage' to update this documentation!
const char* usage_str = ""
"NAME\r\n"
"\r\n"
"    ph.exe - a shell protocol handler to run external programs\r\n"
"\r\n"
"\r\n"
"SYNOPSIS\r\n"
"\r\n"
"    ph.exe <url> [config_file]\r\n"
"    ph.exe -e <config_file> [out_file]\r\n"
"    ph.exe -r </regex/replace/> <file>\r\n"
"    ph.exe -h \r\n"
"\r\n"
"\r\n"
"DESCRIPTION\r\n"
"\r\n"
"    ph.exe can be registered as protocol handler by shells that support \r\n"
"    global custom protocol handlers. The program will expect a complete url as \r\n"
"    first parameter. The authority and path parameter parts of the url are then\r\n"
"    used to look up a section in the configuration file which holds \r\n"
"    instructions what needs to be executed. The config file is written in the \r\n"
"    form of an ini file or it can be compiled into an encoded file to hide \r\n"
"    sensitive information such as passwords.\r\n"
"    \r\n"
"    If the optional [config_file] parameter is provided, the program will \r\n"
"    load the configuration from this location. This might be a UNC path, \r\n"
"    a local file or a remote file from an http/ftp server. If parameter 2 is \r\n"
"    ommited, the programm will first look for a `ph.dat` and then \r\n"
"    for `ph.ini` in the same folder as the executable. See \r\n"
"    CONFIGURATION for mor information.\r\n"
"    \r\n"
"    Once the program is registered to handle a specific protocol scheme, it must\r\n"
"    be called by the shell with the full url as first parameter. As an example\r\n"
"    (assuming the program is registered with the protocol scheme `proto:`):\r\n"
"    \r\n"
"        proto://authority/path/dir?param1=value1\r\n"
"    \r\n"
"    can be mapped to run the following program:\r\n"
"        \r\n"
"        some_executable.exe --file=\"${param1}\"\r\n"
"    \r\n"
"    where `${param1}` will b substitued by the value of the query parameter \r\n"
"    `value1`. Parameter names are case insensitive. \r\n"
"    \r\n"
"    The mapping is done throug the `ph.ini` file. Detailed \r\n"
"    information on how to use the configuration file can be found in \r\n"
"    INSTALLATION section.\r\n"
"    \r\n"
"    But in short, the following configuration would be needed to make the above\r\n"
"    example work:\r\n"
"    \r\n"
"        [authority/path/dir] ; this is the mapping to the url\r\n"
"        cmd = some_executable.exe --file=\"${param1}\"\r\n"
"\r\n"
"    Registering a protocol scheme can be done trough a registry setting on \r\n"
"    windows. An example registrz file should come with this program if not \r\n"
"    run it in a folder without a configuration file and it will be created.\r\n"
"\r\n"
"\r\n"
"OPTIONS\r\n"
"\r\n"
"    -e encode a plain text ini config file. the resulting file will be stored\r\n"
"       in the same folder bit with a file extension of `.dat`. Mone information\r\n"
"       the usage of this option can be found in INSTALL.txt\r\n"
"    \r\n"
"    -r /regex/replace/ will test the build in regex replace mechanism. Mone \r\n"
"       information the usage of this option can be found in INSTALL.txt\r\n"
"\r\n"
"    -h This help documentation.\r\n"
"\r\n"
"INSTALLATION\r\n"
"\r\n"
"    The minimum installation requires the `ph.exe` being placed in \r\n"
"    your desired installation directory. If the program is run once, it will \r\n"
"    produce an example `ph.ini` file in the same folder which is \r\n"
"    the configuration file. It will also create a `ph.log` file \r\n"
"    for debugging and a `ph.reg` for registration as windows \r\n"
"    protocol handler.\r\n"
"\r\n"
"\r\n"
"WINDOWS REGISTRY\r\n"
"\r\n"
"    The `ph.reg` file includes two place holders `<proto>` with your\r\n"
"    desired protocol name. If, say, your shell shall route all calls to \r\n"
"    `myprotocol://someuri` to `ph.exe`, then replace `<proto>` with\r\n"
"    `myprotocol`. You will need to replace `<yourpath>` with the path to the \r\n"
"    `ph.exe` executable. Make sure to use double backslashes `\\\\` in\r\n"
"    the path, a single backslash is interpreted as escape character.\r\n"
"\r\n"
"    The example registry file includes 2 examples how to suppress the warning in\r\n"
"    internet explorer. One for the user registry (per user), one for the local \r\n"
"    machine (system wide).\r\n"
"\r\n"
"\r\n"
"CONFIGURATION\r\n"
"\r\n"
"    The program will first check if it is called with a 2nd parameter. If so, \r\n"
"    it will search the configuration file in this location. This might be a \r\n"
"    local file, an UNC path rl an http/ftp url. If parameter 2 is not provided, \r\n"
"    `ph.exe` will look in the isntallation directory for a file \r\n"
"    called `ph.ini` or `ph.dat`. The latter is an \r\n"
"    obfuscated file which hides sensitive data such as passwords.\r\n"
"\r\n"
"    You can create an obfuscated file by running:\r\n"
"\r\n"
"        ph.exe -e <config_file> [out_file]\r\n"
"\r\n"
"    if parameter 2 is omitted, the obfuscated file will be stored in the same \r\n"
"    directory as the source file but with a `.dat` ending.\r\n"
"\r\n"
"    The configuration file is in INI format and contains sections and \r\n"
"    name=value pairs. The sectiosn are enclosed in brackets and represent the \r\n"
"    authority and path part of an URI. If you, for example, run the following \r\n"
"    url `proto://something/else` through `ph.exe`, the the program \r\n"
"    will look for the following section in the ini file: \r\n"
"    [something/else]. If found, all name value pairs are read and interpreted.\r\n"
"\r\n"
"Configuration directives:\r\n"
"\r\n"
"    cmd: \r\n"
"        The command to execute. This is the main config value which does\r\n"
"        define the full command line. Make sure to use absolute paths.\r\n"
"        \r\n"
"        Variable substitution is available for environment variables\r\n"
"        and URI query parameters. The URI query parameters are represented \r\n"
"        in the following format in the cmd value: `${param_name}` which will\r\n"
"        corespond to the query parameter `?param_name=value` and be expanded\r\n"
"        to `value`. If the query variable cannot be found, the proram will \r\n"
"        abort.\r\n"
"        \r\n"
"        Environment variables can be inserted in the cmd configuration in the \r\n"
"        following format: `${env.APPDATA}`, where `APPDATA` must be a valid \r\n"
"        environment variable. If it cannot be found, the program will abort.\r\n"
"    \r\n"
"    replace_regex: \r\n"
"        It is possible to run a regular expression search&replace against any \r\n"
"        file on this system. Regular expression are deifind in the following \r\n"
"        form /regex/replace/. Back references cane be used in the replace \r\n"
"        syntax and must be prepended by `$` (eg. `$1`).\r\n"
"        \r\n"
"        This program uses the Perl Compatible Regular Expresseion (PCRE) \r\n"
"        syntax. More on PCRE here: \r\n"
"        https://www.rexegg.com/pcre-doc/_latest/pcresyntax.html\r\n"
"    \r\n"
"    replace_file: \r\n"
"        File to run a regex against\r\n"
"\r\n"
"    lpadzero: \r\n"
"        Comma spearated list of url parameters that shall be zeropadded to 9\r\n"
"        digits. The pading will be done right before the command is executed.\r\n"
"\r\n"
"    ltrimzero: \r\n"
"        Comma spearated list of url parameters that shall be left trimmed of \r\n"
"        '0' characters. The stripping will be done right before the command \r\n"
"        is executed.\r\n"
"\r\n"
"    user_param:\r\n"
"        provide an url parmater that will contain a username. The system checks\r\n"
"        if the provided parameter value matches with the currently logged in \r\n"
"        user (case insensitive comparison).\r\n"
"        \r\n"
"        If the user name matches, the command in `cmd_usermatch` will be run,\r\n"
"        otherwise, the ocmmand in `cmd_nousermatch` will be run. the \r\n"
"        `cmd` command will be ignored wif `user_param`.\r\n"
"    \r\n"
"    cmd_usermatch\r\n"
"        If `user_param` is set and the parameters value matches the currently \r\n"
"        logged in user this command will be run. `cmd` will\r\n"
"        be ignored.\r\n"
"    \r\n"
"    cmd_nousermatch\r\n"
"        If `user_param` is set and the parameters value does not match the \r\n"
"        currently logged in user this command will be run. `cmd` will\r\n"
"        be ignored.\r\n"
"    \r\n"
"    precmd:\r\n"
"        This is an optional command that will be run before the actual command \r\n"
"        defined in `cmd`. Variables will be expanded the same way yas described \r\n"
"        in `cmd`. \r\n"
"\r\n"
"    precmd_usermatch:\r\n"
"        If the config parameter `user_param` is used and the user matches, \r\n"
"        this command will be run before `cmd_usermatch` is executed.\r\n"
"\r\n"
"    precmd_bousermatch:\r\n"
"        If the config parameter `user_param` is used and the user matches, \r\n"
"        this command will be run before `cmd_nousermatch` is executed.\r\n"
"\r\n"
"    start_hidden\r\n"
"        If set to 1, the application will be started and the main window is \r\n"
"        hidden (no taskbar entry).\r\n"
"    \r\n"
"    start_minimized\r\n"
"        If set to 1, the program will be launched and minimized into the \r\n"
"        taskbar. `start_hidden` will overrride this feature.\r\n"
"    \r\n"
"    file_must_exist\r\n"
"        Check if a file exists. if not, the program will exit with exit code \r\n"
"        25 and display a custom error message configured in\r\n"
"        `file_must_exist_error`.\r\n"
"        \r\n"
"    file_must_exist_error\r\n"
"        This is the custom error message which is display to the user if \r\n"
"        the file in `file_must_exist` is not found. You may use HTML in this \r\n"
"        message. Beware: the whole html code must be on one line in the ini \r\n"
"        file and may not exceed 4000 characters.\r\n"
"\r\n"
"        \r\n"
"ENVIRONMENT\r\n"
"\r\n"
"    PH_HOME  is set upon startup of the program in it's own process and will \r\n"
"             point to the folder of the executable. This variable can be used \r\n"
"             in the configuration file to point to the application directory.\r\n"
"\r\n"
"\r\n"
"FILES\r\n"
"\r\n"
"    $PH_HOME/ph.ini      The default location of the configration file. If the \r\n"
"                         2nd argument is not provided, `ph.exe`\r\n"
"                         will search for the configuration file here.\r\n"
"\r\n"
"    $PH_HOME/ph.dat      The default location of the encoded configration file.\r\n"
"                         To encode an `.ini` file into a `.dat` file see \r\n"
"                         the `-e` command line switch.\r\n"
"\r\n"
"    ph.log               Logfile of the program. The location of the logfile is\r\n"
"                         detiremined by two environment variables. if the \r\n"
"                         variable APPDATA can be resolved, it will be put there\r\n"
"                         as `ph.log` otherwise the program checks\r\n"
"                         for `HOME` if found, the log file will be stored \r\n"
"                         as `.ph.log`. If none of the two \r\n"
"                         environment variables is available we panic an \r\n"
"                         abort :-/\r\n"
"\r\n"
"    $PH_HOME/error.html  Error messages for display in a browser when the \r\n"
"                         program encounters a problem.\r\n"
"\r\n"
"\r\n"
"LIMITATIONS\r\n"
"\r\n"
"    1. The maximung length of a command (with all it's parameters) is limited \r\n"
"       to 260 bytes.\r\n"
"\r\n"
"    2. the authority and path part in the URI are restricted to the following \r\n"
"       characters as per rfc3986: [a-zA-Z0-9-._~] and in the path part \r\n"
"       additionally '/' is allowed. This programm will parse any value but the \r\n"
"       shell might handle any other character in an unpredictable way, so avoid\r\n"
"       them in ini sections.\r\n"
"\r\n"
"    3. Fully qualified path's to executables must be used in the the ini file's \r\n"
"       `exe` directive. You might get lucky by using relative paths, but this \r\n"
"       can be a security concern.\r\n"
"\r\n"
"    4. It is unclear how this program will behave on multibyte character sets \r\n"
"       coming from user input. It will probably crash and burn your computer \r\n"
"       and desk down. Huzzah!\r\n"
"    \r\n"
"    5. The line length in the ini file is limited to max 4096 characters.\r\n"
"\r\n"
"\r\n"
"COPYRIGHT\r\n"
"\r\n"
"    2017, Simon Wunderlin (BSD license)\r\n"
"    2009, Ben Hoyt - Ini parser, https://github.com/benhoyt/inih (BSD license)\r\n"
"    1996 - 2017 Daniel Stenberg, daniel@haxx.se - cURL (BSD license)\r\n"
"    \r\n"
"    Regular expression support is provided by the PCRE library package,\r\n"
"    which is open source software, written by Philip Hazel, and copyright\r\n"
"    by the University of Cambridge, England.\r\n"
"\r\n"
"    Copyright OpenSSL 2017\r\n"
"    Contents licensed under the terms of the OpenSSL license\r\n"
"    See http://www.openssl.org/source/license.html for details\r\n"
"\r\n"
"SEE ALSO\r\n"
"\r\n"
"    INSTALL.txt        for installation and configuration instructions.\r\n"
"    BUILD.txt          for instructions on building for Windows, OSX and Linux\r\n"
"    LICENSE.txt        BSD License\r\n"
"    ph.reg             Example registry file\r\n"
"    ph.ini             Configuration file\r\n"
"    ph.dat             Encoded configuration file\r\n"
"";
