#! /bin/bash
BIN=/bin
UPATH=$(HOME)/.littlebenchmark
MANPATH=/usr/local/man/man1

build:
	make -C src
	make -C doc
clean:
	make -C src clean
	make -C doc clean
makedoc:
	make -C doc
install:
	cp src/littlebenchmark $(BIN)
	mkdir -p $(UPATH)/doc
	cp doc/ReadMe.pdf doc/refman/latex/refman.pdf $(UPATH)/doc
	cp -r scripts $(UPATH)
	mkdir -p $MANPATH
	cp man/LittleBenchmark_HDD.1 $MANPATH
uninstall:
	@if [ -e $(BIN)/littlebenchmark ]; then rm $(BIN)/littlebenchmark; echo "rm $(BIN)/littlebenchmark"; fi
	@if [ -f $(UPATH)/doc ]; then rm $(UPATH)/doc; echo "rm -r $(UPATH)/doc"; fi
	@if [ -f $(UPATH)/scripts ]; then rm  $(UPATH)/scripts; echo "rm -r $(UPATH)/scripts"; fi
purge:
	@if [ -e $(BIN)/littlebenchmark ]; then rm $(BIN)/littlebenchmark; echo "rm $(BIN)/littlebenchmark"; fi
	@if [ -f $(UPATH) ]; then rm  $(UPATH); echo "rm -r $(UPATH)"; fi

.PHONY: build clean makedoc install uninstall purge
