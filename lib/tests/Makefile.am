###########################################################################
##
## File:        ./lib/tests/Makefile.am
## Versions:    $Id: Makefile.am,v 1.1 2002/07/18 22:30:57 rkowen Exp $
## Created:     2002/06/14
##
###########################################################################
##
## run  automake to generate a Makefile.in
##	Makefile.am \
##	configure.ac >  Makefile.in
##	aclocal.m4  /
##

AUTOMAKE_OPTIONS=foreign no-installinfo no-installman

noinst_PROGRAMS = tavec.x tiprime.x tisqrt.x tuvec.x tstrmalloc.x

LDADD		= ../libmodules.a

tavec_SOURCES		= tavec.c
tiprime_SOURCES		= tiprime.c
tisqrt_SOURCES		= tisqrt.c
tuvec_SOURCES		= tuvec.c
tstrmalloc_SOURCES	= tstrmalloc.c

help : 
	@echo "Targets:"
	@echo "	all	- create all the test executables"
#	@echo "	mall	- create all the test memory debug executables"
	@echo "	runall	- run all the tests"
	@echo "	test	- both of above"
	@echo "	help	- this info"
	@echo "	clean	- remove .o files"
	@echo "	clobber	- do clean and remove all executables"
	
EXES = $(noinst_PROGRAMS)

runall : $(EXES)
	-for f in $(EXES) ; do echo ===$$f===; ./$$f ; done 2>&1 | more

test : all runall

clean :
	-rm -f *.o *_m.o a.out core

clobber : clean
	-rm -f $(EXES)
