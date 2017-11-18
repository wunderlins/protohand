NAME

    PROGNAME_SHORT_EXT - a shell protocol handler to run external programs


SYNOPSIS

    PROGNAME_SHORT_EXT <url> [config_file]
    PROGNAME_SHORT_EXT -e <config_file> [out_file]
    PROGNAME_SHORT_EXT -r </regex/replace/> <file>
    PROGNAME_SHORT_EXT -h 


DESCRIPTION

    PROGNAME_SHORT_EXT can be registered as protocol handler by shells that support 
    global custom protocol handlers. The program will expect a complete url as 
    first parameter. The authority and path parameter parts of the url are then
    used to look up a section in the configuration file which holds 
    instructions what needs to be executed. The config file is written in the 
    form of an ini file or it can be compiled into an encoded file to hide 
    sensitive information such as passwords.
    
    If the optional [config_file] parameter is provided, the program will 
    load the configuration from this location. This might be a UNC path
    or local file. If parameter 2 is ommited, the programm will first look 
    for a `PROGNAME_SHORT.dat` and then for `PROGNAME_SHORT.ini` in the same 
    folder as the executable. See CONFIGURATION for mor information.
    
    Once the program is registered to handle a specific protocol scheme, it must
    be called by the shell with the full url as first parameter. As an example
    (assuming the program is registered with the protocol scheme `proto:`):
    
        proto://authority/path/dir?param1=value1
    
    can be mapped to run the following program:
        
        some_executable.exe --file="${param1}"
    
    where `${param1}` will b substitued by the value of the query parameter 
    `value1`. Parameter names are case insensitive. 
    
    The mapping is done throug the `PROGNAME_SHORT.ini` file. Detailed 
    information on how to use the configuration file can be found in 
    INSTALLATION section.
    
    But in short, the following configuration would be needed to make the above
    example work:
    
        [authority/path/dir] ; this is the mapping to the url
        cmd = some_executable.exe --file="${param1}"

    Registering a protocol scheme can be done trough a registry setting on 
    windows. An example registrz file should come with this program if not 
    run it in a folder without a configuration file and it will be created.


OPTIONS

    -e encode a plain text ini config file. the resulting file will be stored
       in the same folder bit with a file extension of `.dat`. Mone information
       the usage of this option can be found in INSTALL.txt
    
    -r /regex/replace/ will test the build in regex replace mechanism. Mone 
       information the usage of this option can be found in INSTALL.txt

    -h This help documentation.

INSTALLATION

    The minimum installation requires the `PROGNAME_SHORT_EXT` being placed in 
    your desired installation directory. If the program is run once, it will 
    produce an example `PROGNAME_SHORT.ini` file in the same folder which is 
    the configuration file. It will also create a `PROGNAME_SHORT.log` file 
    for debugging and a `PROGNAME_SHORT.reg` for registration as windows 
    protocol handler.


WINDOWS REGISTRY

    The `PROGNAME_SHORT.reg` file includes two place holders `<proto>` with your
    desired protocol name. If, say, your shell shall route all calls to 
    `myprotocol://someuri` to `PROGNAME_SHORT_EXT`, then replace `<proto>` with
    `myprotocol`. You will need to replace `<yourpath>` with the path to the 
    `PROGNAME_SHORT_EXT` executable. Make sure to use double backslashes `\\` in
    the path, a single backslash is interpreted as escape character.

    The example registry file includes 2 examples how to suppress the warning in
    internet explorer. One for the user registry (per user), one for the local 
    machine (system wide).


CONFIGURATION

    The program will first check if it is called with a 2nd parameter. If so, it
    will search the configuration file in this locations. This might be a local 
    file path or an UNC. If parameter 2 is not provided, `PROGNAME_SHORT_EXT` 
    will     look in the isntallation directory for a file called 
    `PROGNAME_SHORT.ini` or `PROGNAME_SHORT.dat`. The latter is an obfuscated 
    file which hides sensitive data such as passwords.

    You can create an obfuscated file by running:

        PROGNAME_SHORT_EXT -e <config_file> [out_file]

    if parameter 2 is omitted, the obfuscated file will be stored in the same 
    directory as the source file but with a `.dat` ending.

    The configuration file is in INI format and contains sections and 
    name=value pairs. The sectiosn are enclosed in brackets and represent the 
    authority and path part of an URI. If you, for example, run the following 
    url `proto://something/else` through `PROGNAME_SHORT_EXT`, the the program 
    will look for the following section in the ini file: 
    [something/else]. If found, all name value pairs are read and interpreted.

Configuration directives:

    cmd: 
        The command to execute. This is the main config value which does
        define the full command line. Make sure to use absolute paths.
        
        Variable substitution is available for environment variables
        and URI query parameters. The URI query parameters are represented 
        in the following format in the cmd value: `${param_name}` which will
        corespond to the query parameter `?param_name=value` and be expanded
        to `value`. If the query variable cannot be found, the proram will 
        abort.
        
        Environment variables can be inserted in the cmd configuration in the 
        following format: `${env.APPDATA}`, where `APPDATA` must be a valid 
        environment variable. If it cannot be found, the program will abort.
    
    replace_regex: 
        It is possible to run a regular expression search&replace against any 
        file on this system. Regular expression are deifind in the following 
        form /regex/replace/. Back references cane be used in the replace 
        syntax and must be prepended by `$` (eg. `$1`).
        
        This program uses the Perl Compatible Regular Expresseion (PCRE) 
        syntax. More on PCRE here: 
        https://www.rexegg.com/pcre-doc/_latest/pcresyntax.html
    
    replace_file: 
        File to run a regex against
    

ENVIRONMENT

    PH_HOME  is set upon startup of the program in it's own process and will 
             point to the folder of the executable. This variable can be used 
             in the configuration file to point to the application directory.


FILES

    $PH_HOME/ph.ini      The default location of the configration file. If the 
                         2nd argument is not provided, `PROGNAME_SHORT_EXT`
                         will search for the configuration file here.

    $PH_HOME/ph.dat      The default location of the encoded configration file.
                         To encode an `.ini` file into a `.dat` file see 
                         the `-e` command line switch.

    ph.log               Logfile of the program. The location of the logfile is
                         detiremined by two environment variables. if the 
                         variable APPDATA can be resolved, it will be put there
                         as `PROGNAME_SHORT.log` otherwise the program checks
                         for `HOME` if found, the log file will be stored 
                         as `.PROGNAME_SHORT.log`. If none of the two 
                         environment variables is available we panic an 
                         abort :-/

    $PH_HOME/error.html  Error messages for display in a browser when the 
                         program encounters a problem.


LIMITATIONS

    1. The maximung length of a command (with all it's parameters) is limited 
       to MAX_CWD_LENGTH bytes.

    2. the authority and path part in the URI are restricted to the following 
       characters as per rfc3986: [a-zA-Z0-9-._~] and in the path part 
       additionally '/' is allowed. This programm will parse any value but the 
       shell might handle any other character in an unpredictable way, so avoid
       them in ini sections.

    3. Fully qualified path's to executables must be used in the the ini file's 
       `exe` directive. You might get lucky by using relative paths, but this 
       can be a security concern.

    4. It is unclear how this program will behave on multibyte character sets 
       coming from user input. It will probably crash and burn your computer 
       and desk down. Huzzah!


COPYRIGHT

    2017, Simon Wunderlin (BSD license)
    2009, Ben Hoyt - Ini parser, https://github.com/benhoyt/inih (BSD license)
	1996 - 2017 Daniel Stenberg, daniel@haxx.se - cURL (BSD license)
    
    Regular expression support is provided by the PCRE library package,
    which is open source software, written by Philip Hazel, and copyright
    by the University of Cambridge, England.

    Copyright OpenSSL 2017
    Contents licensed under the terms of the OpenSSL license
    See http://www.openssl.org/source/license.html for details

SEE ALSO

    INSTALL.txt        for installation and configuration instructions.
    BUILD.txt          for instructions on building for Windows, OSX and Linux
    LICENSE.txt        BSD License
    PROGNAME_SHORT.reg             Example registry file
    PROGNAME_SHORT.ini             Configuration file
    PROGNAME_SHORT.dat             Encoded configuration file
