###########################################################################
##
## File:        ./lib/tests/Makefile.am
## Versions:    $Id: Makefile.am,v 1.3 2002/09/18 05:47:48 harlan Exp $
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

EXTRA_PROGRAMS = tavec tiprime tisqrt tuvec tstrmalloc

LDADD		= ../libmodules.a

# tavec_SOURCES		= tavec.c
# tiprime_SOURCES		= tiprime.c
# tisqrt_SOURCES		= tisqrt.c
# tuvec_SOURCES		= tuvec.c
# tstrmalloc_SOURCES	= tstrmalloc.c

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
