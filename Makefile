.PHONY: all test

all:
	$(MAKE) release -C src

test:
	$(MAKE) test -C src

clean:
	$(MAKE) clean -C src
