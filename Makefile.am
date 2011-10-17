###########################################################################
##
## File:        ./Makefile.am
## Versions:    $Id: Makefile.am,v 1.21.6.2 2011/10/17 17:16:50 rkowen Exp $
## Created:     2002/06/14
##
###########################################################################
##
## run  automake to generate a Makefile.in
##	Makefile.am \
##	configure.ac >  Makefile.in
##	aclocal.m4  /
##

ACLOCAL_AMFLAGS= -I config

man_MANS = doc/module.1 doc/modulefile.4
SUBDIRS = . init modulefiles @EXTDIR@ doc
DIST_SUBDIRS = . init modulefiles etc ext doc
SOURCES_H = modules_def.h getopt.h config.h
EXTRA_DIST = modules_def.h getopt.h testsuite INSTALL.RH7x		\
	init/filter init/bash.in init/sh.in init/zsh.in			\
	init/csh.in init/ksh.in init/perl.pm.in init/python.py.in	\
	init/ruby.rb.in init/cmake.in init/bash_completion.in		\
	LICENSE.GPL

bin_PROGRAMS = modulecmd
# bin_SCRIPTS = etc/mkroot etc/add.modules ext/add.ext
modulecmd_CFLAGS=@TCLX_INCLUDE_SPEC@  @TCL_INCLUDE_SPEC@ @X_CFLAGS@
modulecmd_LDADD= @TCLX_LIB_SPEC@ @TCL_LIB_SPEC@ @X_LIBS@ @X_EXTRA_LIBS@ @LIBS@
modulecmd_SOURCES = \
	ModuleCmd_Avail.c \
        ModuleCmd_Clear.c ModuleCmd_Display.c ModuleCmd_Help.c \
	ModuleCmd_Init.c ModuleCmd_List.c ModuleCmd_Load.c ModuleCmd_Purge.c \
	ModuleCmd_Switch.c ModuleCmd_Update.c ModuleCmd_Use.c \
	ModuleCmd_Whatis.c ModuleCmd_Refresh.c \
	cmdAlias.c cmdConflict.c cmdIsLoaded.c cmdInfo.c cmdMisc.c cmdModule.c\
	cmdPath.c cmdSetenv.c cmdUname.c cmdXResource.c cmdUlvl.c cmdLog.c \
	cmdChdir.c cmdTrace.c cmdVersion.c cmdVerbose.c cmdWhatis.c		\
	init.c locate_module.c utility.c main.c error.c getopt.c version.c

test: check-local

check-local:	modulecmd
	MODULEVERSION=@VERSION@; export MODULEVERSION; \
	TESTSUITEDIR=$(srcdir)/testsuite; export TESTSUITEDIR; \
	$(RUNTEST) --srcdir $$TESTSUITEDIR \
	$(AM_RUNTESTFLAGS) $(RUNTESTFLAGS) --tool=modules

ctags:	CTAGS
CTAGS:	$(modulecmd_SOURCES)
	ctags	$(modulecmd_SOURCES) $(SOURCES_H)

cxref:
	$(MAKE) clean
#	may need to edit the cxref-cc script setting
#	EGREP=/bin/egrep
	$(MAKE) CC=cxref-cc CXREFCC=$(CC)
#	cxref-cc $(modulecmd_SOURCES) $(SOURCES_H)
	@echo "==============================="
	@echo "You may need to run this twice!"
	@echo "==============================="

SED=sed
CVS=cvs
GZIP=gzip
BZIP2=bzip2

patch: $(srcdir)/configure
	@echo "Creating patch file -- do this after CVS tagging"
	${CVS} rdiff -c \
	 -r `${SED} -n -e 's/^.*OLD TAG.*"\(.*\)".*$$/\1/p' ${srcdir}/version.c` \
	 -r `${SED} -n -e 's/^.*NEW TAG.*"\(.*\)".*$$/\1/p' ${srcdir}/version.c` \
	modules | ${GZIP} -c \
	> modules-`${SED} -n -e 's/^.*OLD TAG.*"modules-\([0-9]*\)-\([0-9]*\)-\([0-9]*\)".*$$/\1.\2.\3/p' ${srcdir}/version.c`-`${SED} -n -e 's/^.*NEW TAG.*"modules-\([0-9]*\)-\([0-9]*\)-\([0-9]*\)".*$$/\1.\2.\3/p' ${srcdir}/version.c`.diff.gz
	${CVS} rdiff -c \
	 -r `${SED} -n -e 's/^.*OLD TAG.*"\(.*\)".*$$/\1/p' ${srcdir}/version.c` \
	 -r `${SED} -n -e 's/^.*NEW TAG.*"\(.*\)".*$$/\1/p' ${srcdir}/version.c` \
	modules | ${BZIP2} -c \
	> modules-`${SED} -n -e 's/^.*OLD TAG.*"modules-\([0-9]*\)-\([0-9]*\)-\([0-9]*\)".*$$/\1.\2.\3/p' ${srcdir}/version.c`-`${SED} -n -e 's/^.*NEW TAG.*"modules-\([0-9]*\)-\([0-9]*\)-\([0-9]*\)".*$$/\1.\2.\3/p' ${srcdir}/version.c`.diff.bz2

tag:
	@echo "CVS tagging - TAG = "\
	  `${SED} -n -e 's/^.*NEW TAG.*"\(.*\)".*$$/\1/p' ${srcdir}/version.c`
	${CVS} rtag -b -F \
	  `${SED} -n -e 's/^.*NEW TAG.*"\(.*\)".*$$/\1/p' ${srcdir}/version.c` \
	  modules

CL: rmChangeLog
	@echo "Updating ChangeLog"
	-@./cvs2cl.pl

rmChangeLog:
	-@rm -f ChangeLog


lsm:
# sends off the modules.lsm file to update the Linux Software Map
# this should be done by the maintainer (R.K.Owen) only!
	mail -s add modules@kooz.sj.ca.us,lsm@execpc.com < modules.lsm

DISTNAME=$(distdir)

ftp-local: dist
# this should be done by the maintainer (R.K.Owen) only!
	@echo "placing $(DISTNAME).tar.{gz|bz2}"
	-@if [ -e $(DISTNAME).tar.gz ]; then \
		cp $(DISTNAME).tar.gz /u/ftp/pub/rkowen/modules; \
	else echo "Can't find $(DISTNAME).tar.gz"; fi
	-@if [ -e $(DISTNAME).tar.bz2 ]; then \
		cp $(DISTNAME).tar.bz2 /u/ftp/pub/rkowen/modules; \
	else echo "Can't find $(DISTNAME).tar.bz2"; fi
	-@XXX=modules-`${SED} -n -e 's/^.*OLD TAG.*"modules-\([0-9]*\)-\([0-9]*\)-\([0-9]*\)".*$$/\1.\2.\3/p' ${srcdir}/version.c`-`${SED} -n -e 's/^.*NEW TAG.*"modules-\([0-9]*\)-\([0-9]*\)-\([0-9]*\)".*$$/\1.\2.\3/p' ${srcdir}/version.c`.diff.gz ; \
	echo "placing $$XXX"; \
	if [ -e $$XXX ]; then \
		cp $$XXX /u/ftp/pub/rkowen/modules; \
	else echo "Can't find $$XXX"; fi
	-@XXX=modules-`${SED} -n -e 's/^.*OLD TAG.*"modules-\([0-9]*\)-\([0-9]*\)-\([0-9]*\)".*$$/\1.\2.\3/p' ${srcdir}/version.c`-`${SED} -n -e 's/^.*NEW TAG.*"modules-\([0-9]*\)-\([0-9]*\)-\([0-9]*\)".*$$/\1.\2.\3/p' ${srcdir}/version.c`.diff.bz2 ; \
	echo "placing $$XXX"; \
	if [ -e $$XXX ]; then \
		cp $$XXX /u/ftp/pub/rkowen/modules; \
	else echo "Can't find $$XXX"; fi
	-@echo "placing modules.lsm"
	-@if [ -e modules.lsm ]; then \
		cp modules.lsm /u/ftp/pub/rkowen/modules; \
	else echo "Can't find modules.lsm"; fi

dist-hook:
	-rm -rf `find $(distdir) -name CVS`
	-touch $(distdir)/testsuite/modulefiles/trace/CVS

ftp: ftp-ibiblio ftp-sourceforge

ftp-ibiblio: dist
# do local files first ... mostly to verify their existence
# this should be done by the maintainer (R.K.Owen) only!
	@echo "placing ftp.ibiblio.org files";
	-@XXX=modules-`${SED} -n -e 's/^.*OLD TAG.*"modules-\([0-9]*\)-\([0-9]*\)-\([0-9]*\)".*$$/\1.\2.\3/p' ${srcdir}/version.c`-`${SED} -n -e 's/^.*NEW TAG.*"modules-\([0-9]*\)-\([0-9]*\)-\([0-9]*\)".*$$/\1.\2.\3/p' ${srcdir}/version.c`.diff."*" ; \
	echo "placing modules.lsm $$XXX $(DISTNAME).tar.*"; \
	./.ftp ftp.ibiblio.org /incoming/linux \
		modules.lsm $$XXX $(DISTNAME).tar.*;

ftp-sourceforge: dist
	@echo "placing modules.sourceforge.net files";
	-@XXX=modules-`${SED} -n -e 's/^.*OLD TAG.*"modules-\([0-9]*\)-\([0-9]*\)-\([0-9]*\)".*$$/\1.\2.\3/p' ${srcdir}/version.c`-`${SED} -n -e 's/^.*NEW TAG.*"modules-\([0-9]*\)-\([0-9]*\)-\([0-9]*\)".*$$/\1.\2.\3/p' ${srcdir}/version.c`.diff."*" ; \
	echo "placing modules.lsm $$XXX $(DISTNAME).tar."*""; \
	scp modules.lsm $$XXX $(DISTNAME).tar.* rkowen@shell.sourceforge.net:ftp
	./.ftp upload.sourceforge.net /incoming \
		modules.lsm $$XXX $(DISTNAME).tar.*;

help:
	@echo ""
	@echo "make           - compiles sources to executable"
	@echo "make all       - same as above"
	@echo "make check     - runs test suite - needs dejagnu runtest"
	@echo "make install   - copy program & man pages to destination"
	@echo "make clean     - cleans out most useless files"
	@echo "make disthelp  - extra help for developers"
	@echo ""

disthelp:
	@echo "make ctags     - creates the vi ctags file"
	@echo "make cxref     - makes the xref HTML files - needs cxref"
	@echo "make dist      - creates a distribution tarballs"
	@echo "make distclean - cleans & removes most made files"
	@echo "make maintainer-clean - cleans even more"
	@echo "================ requires CVS access ======================="
	@echo "make tag       - does a CVS rtag"
	@echo "make CL        - remakes the ChangeLog from the CVS commits"
	@echo "make patch     - creates a patch file after tagging"
	@echo "================ maintainers only =========================="
	@echo "make lsm       - send off to the Linux Software Map"
	@echo "make ftp-local - upload tarball (good test)"
	@echo "make ftp-ibiblio      - upload to ibiblio"
	@echo "make ftp-sourceforge  - upload to sourceforge"
	@echo "make ftp       - upload to both ibiblio & sourceforge"
	@echo ""

