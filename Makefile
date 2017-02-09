export MODULESHOME = $(shell pwd)
.PHONY: doc www initdir clean

all: initdir doc

initdir:
	make -C init all

doc:
	make -C doc all

www:
	make -C www all

distclean: clean

clean: 
	rm -f *.log *.sum
	make -C init clean
	make -C doc clean
	make -C www clean

test:
	MODULEVERSION=Tcl; export MODULEVERSION; \
	OBJDIR=`pwd -P`; export OBJDIR; \
	TESTSUITEDIR=`cd testsuite;pwd -P`; export TESTSUITEDIR; \
	runtest --srcdir $$TESTSUITEDIR --objdir $$OBJDIR --tool modules -v
