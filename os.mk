# on darwin, we need to add a file extension to the -i flag of sed

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