.PHONY: all test

all:
	$(MAKE) all release -C src DEBUG=0

test:
	$(MAKE) test -C src

clean:
	$(MAKE) clean -C src
