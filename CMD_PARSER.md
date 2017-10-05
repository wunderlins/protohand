# command line parser

The parser shoud replace the exe directive in the ini file. The difference is
that the cmd will hold program an parameter for the command to run. The
parameters will be denoted as variable üplaceholder like ${variable} and 
be replaced by 3 actions:
    - url parameter expansion, syntax: ${paramname}. Paramname will be passed in as query parameter like ?paramname=value
	- environment variable expansion, syntax ${env.varname}, where varname might be any environment variable
	- conditional expansion. insert a value into the command if a passed invariable matches an environment variable like ${env.USERNAME!=user:/u generic}

Example:

    ph://cmd/notepad?user=abc&file=file.txt


cmd directive:

    cmd = ${env.windir}\notepad.exe /A "${file}" ${env.USERNAME!=user:--debug}
	
result (if the windows user equals to "abc"):

	c:\windows\notepad.exe /A "file.txt"
	
result (if the windows user does not equal to "abc"):

	c:\windows\notepad.exe /A "file.txt" --debug
