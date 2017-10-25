.PHONY: all test

all:
	$(MAKE) all release -C src

test:
	$(MAKE) test -C src

clean:
	$(MAKE) clean -C src
