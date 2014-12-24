export MODULESHOME = $(shell pwd)
.PHONY: doc

all: initdir

initdir:
	make -C init all

doc:
	rm -f ./doc/html/module.html
	pod2html --noindex --infile=./doc/source/module.pl \
		--outfile=./doc/html/module.html
	rm -f ./doc/html/modulefile.html
	pod2html --noindex --infile=./doc/source/modulefile.pl \
		--outfile=./doc/html/modulefile.html
	rm -f ./doc/man/man1/module.1
	pod2man -c "Environment Modules" -r "modules-tcl" \
		./doc/source/module.pl \
		./doc/man/man1/module.1
	rm -f ./doc/man/man4/modulefile.4
	pod2man -c "Environment Modules" -r "modules-tcl" \
		./doc/source/modulefile.pl \
		./doc/man/man4/modulefile.4
	rm -f pod*.tmp

distclean: clean

clean: 
	rm -f *.log *.sum
	make -C init clean

test:
	MODULEVERSION=Tcl; export MODULEVERSION; \
	MODULECMD="modulecmd.tcl"; export MODULECMD; \
	OBJDIR=`pwd`; export OBJDIR; \
	TESTSUITEDIR=`cd testsuite;pwd`; export TESTSUITEDIR; \
	runtest --srcdir $$TESTSUITEDIR --objdir $$OBJDIR --tool modules -v
