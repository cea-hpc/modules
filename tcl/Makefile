all: clean initdir test

distclean: clean

initdir:
	make -C init all

clean: 
	rm -f *.log *.sum
	make -C init clean

test:
	MODULEVERSION=3.1.1; export MODULEVERSION; \
	OBJDIR=`pwd`; export OBJDIR; \
	TESTSUITEDIR=`cd ../testsuite;pwd`; export TESTSUITEDIR; \
	runtest --srcdir $$TESTSUITEDIR --objdir $$OBJDIR --tool modules
