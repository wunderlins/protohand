# on darwin, we need to add a file extension to the -i flag of sed
detected_OS := $(shell sh -c 'uname -s 2>/dev/null || echo not')
SEDI_EXT=
ifeq ($(detected_OS),Darwin)  # Mac OS X
	SEDI_EXT = .bak 
endif

# limits
STDIN_MAX = 1024
MAX_CWD_LENGTH = 1024

# cflags, strip if debuggign is disabled
CFLAGS = -DLOG_TO_FILE=$(LOG_TO_FILE) -Wall -DDEBUG=$(DEBUG) -DPROGNAME=$(PROGNAME) -DSTDIN_MAX=$(STDIN_MAX) -DMAX_CWD_LENGTH=$(MAX_CWD_LENGTH)
ifeq ($(DEBUG),0) 
	CFLAGS += -s
endif

# OS DETECTION
operating_system=
ifeq ($(OS),Windows_NT)
    operating_system=WIN32
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        operating_system=WIN_AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            operating_system=WIN_AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            operating_system=WIN_IA32
        endif
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        operating_system=LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        operating_system=OSX
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        operating_system=AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        operating_system=IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        operating_system=ARM
    endif
endif