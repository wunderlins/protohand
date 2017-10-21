NAME

    PROGNAME_SHORT_EXT - a shell protocol handler to run external programs


SYNOPSIS

    PROGNAME_SHORT_EXT <url> [config_file]
    PROGNAME_SHORT_EXT -e <config_file> [out_file]
	PROGNAME_SHORT_EXT -r </regex/replace/> <file>


DESCRIPTION

    PROGNAME_SHORT_EXT can be registered as protocol handler by shels that support 
    global custom protocol handlers. The program will expect a complete url as 
    first parameter. The authority and path parameter parts of the url are then
    used to look up a section in the configuration file which holds 
    instructions what needs to be executed. The config file is written i nthe 
    form of an ini file or it an be compiled into an encoded file to hide 
    sensitive information such as passwords.


OPTIONS

    -e encode a plain text ini config file. the resulting file will be stored
       in the same folder bit with a file extension of `.dat`.
    
    -r /regex/replace/ will test the build in regex replace mechanism


ENVIRONMENT

    PH_HOME  is set upon startup of the program in it's own process and will 
             point to the folder of the executable. This variable can be used 
             in the configuration file to point to the application directory.


FILES

    $PH_HOME/ph.ini   The default location of the configration file. If the 
                      2nd argument is not provided, `PROGNAME_SHORT_EXT`
                      will search for the configuration file here.

    $PH_HOME/ph.dat   The default location of the encoded configration file.
                      To encode an `.ini` file into a `.dat` file see the `-e`
                      command line switch.

LIMITATIONS

    1. The maximung length of a command (with all it's parameters) is limited 
       to MAX_CWD_LENGTH bytes.

    2. the authority and path part in the URI are restricted to the following 
       characters as per rfc3986: [a-zA-Z0-9-._~] and in the path part 
       additionally '/' is allowed.

    3. url parts are matched with the ini section case sesitive. if you call
       protohand://document/myapp but your section in the ini is called 
       [document/MyApp], the section will not be found.

    4. Fully qualified path's to executables must be used in the the ini file's 
       `exe` directive. You might get lucky by using relative paths, but this 
       can be a security concern.

    5. It is unclear how this program will behave on multibyte character sets 
       coming from user input. It will probably crash and burn your computer 
       and desk down. Huzzah!


COPYRIGHT

    2017, Simon Wunderlin - BSD


SEE ALSO

    INSTALL.txt        for installation instructions.
    CONFIGURATION.txt  descriotion how to configure the program
    BUILD.txt          for instructions on building for Windows, OSX and Linux
    LICENSE.txt        BSD License