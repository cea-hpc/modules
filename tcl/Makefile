all: initdir

distclean: clean

initdir:
	make -C init all

clean: 
	rm -f *.log *.sum
	make -C init clean

test:
	MODULEVERSION=TCL; export MODULEVERSION; \
	MODULECMD="../tcl/modulecmd.tcl"; export MODULECMD; \
	OBJDIR=`pwd`; export OBJDIR; \
	TESTSUITEDIR=`cd ../testsuite;pwd`; export TESTSUITEDIR; \
	runtest --srcdir $$TESTSUITEDIR --objdir $$OBJDIR --tool modules -v
