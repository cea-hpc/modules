AUTOMAKE_OPTIONS=foreign dejagnu no-installinfo

ACLOCAL_AMFLAGS=-I config

bin_PROGRAMS = modulecmd
modulecmd_SOURCES = \
	ModuleCmd_Avail.c ModuleCmd_Bootstrap.c \
        ModuleCmd_Clear.c ModuleCmd_Display.c ModuleCmd_Help.c \
	ModuleCmd_Init.c ModuleCmd_List.c ModuleCmd_Load.c ModuleCmd_Purge.c \
	ModuleCmd_Switch.c ModuleCmd_Update.c ModuleCmd_Use.c \
	ModuleCmd_Whatis.c \
	cmdAlias.c cmdConflict.c cmdIsLoaded.c cmdInfo.c cmdMisc.c cmdModule.c\
	cmdPath.c cmdSetenv.c cmdUname.c cmdXResource.c cmdUlvl.c cmdLog.c \
	cmdTrace.c cmdVersion.c cmdVerbose.c cmdWhatis.c		\
	init.c locate_module.c utility.c main.c error.c getopt.c version.c

#aclocal.m4 : acinclude.m4
#	aclocal -I config

# Makefile.in : Makefile.am aclocal.m4
# 	automake --foreign --add-missing

test: check

check:	modulecmd
	MODULEVERSION=@VERSION@; export MODULEVERSION; TESTSUITEDIR=$(srcdir)/testsuite; export TESTSUITEDIR; $(RUNTEST) --srcdir $$TESTSUITEDIR $(RUNTESTFLAGS) --tool=modules

cxref:
	cxref $(SRCS)
	@echo "==============================="
	@echo "You may need to run this twice!"
	@echo "==============================="

dist: distgz distbz2

distgz : ${DISTNAME}.tar.gz

distbz2 : ${DISTNAME}.tar.bz2

${DISTNAME}.tar.gz : $(srcdir)/configure 
	@echo "checking out ${DISTNAME} into directory modules"
	${CVS} -f checkout -r \
	 `${SED} -n -e 's/^.*NEW TAG.*"\(.*\)".*$$/\1/p' ${srcdir}/version.c` \
	 modules
	@echo "renaming directory to ${DISTNAME}"
	mv modules ${DISTNAME}
	@echo "cleaning out unnecessary directories in ${DISTNAME}"
	${FIND} ${DISTNAME} -type d -name CVS -print | ${XARGS} ${RM} -rf
# need this CVS to pass testsuite for distribution build
	mkdir ${DISTNAME}/testsuite/modulefiles/trace/CVS
	@echo "Creating ${DISTNAME}.tar.gz"
	${TAR} chf - ${DISTNAME} | ${GZIP} -c --best >${DISTNAME}.tar.gz
	@echo "removing directory ${DISTNAME}"
	${RM} -r ${DISTNAME}

${DISTNAME}.tar.bz2 : ${DISTNAME}.tar.gz
	@echo "Creating ${DISTNAME}.tar.bz2"
	${GZIP} -d -c ${DISTNAME}.tar.gz | ${BZIP2} > ${DISTNAME}.tar.bz2

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
	${CVS} rtag -F \
	  `${SED} -n -e 's/^.*NEW TAG.*"\(.*\)".*$$/\1/p' ${srcdir}/version.c` \
	  modules

lsm:
# sends off the modules.lsm file to update the Linux Software Map
# this should be done by the maintainer (R.K.Owen) only!
	mail -s add modules@kooz.sj.ca.us,lsm@execpc.com < modules.lsm

ftp-local:
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

ftp: ftp-sunsite ftp-sourceforge

ftp-sunsite:
# do local files first ... mostly to verify their existence
# this should be done by the maintainer (R.K.Owen) only!
	@echo "placing sunsite.unc.edu files";
	-@XXX=modules-`${SED} -n -e 's/^.*OLD TAG.*"modules-\([0-9]*\)-\([0-9]*\)-\([0-9]*\)".*$$/\1.\2.\3/p' ${srcdir}/version.c`-`${SED} -n -e 's/^.*NEW TAG.*"modules-\([0-9]*\)-\([0-9]*\)-\([0-9]*\)".*$$/\1.\2.\3/p' ${srcdir}/version.c`.diff."*" ; \
	echo "placing modules.lsm $$XXX $(DISTNAME).tar.*"; \
	./.ftp metalab.unc.edu /incoming/Linux \
		modules.lsm $$XXX $(DISTNAME).tar.*;

ftp-sourceforge:
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
	@echo "make distclean - cleans & removes most made files"
	@echo "make disthelp  - extra help for developers"
	@echo ""

disthelp:
	@echo "make ctags     - creates the vi ctags file"
	@echo "make cxref     - makes the xref HTML files - needs cxref"
	@echo "================ requires CVS access ======================="
	@echo "make tag       - does a CVS rtag"
	@echo "make dist      - creates a distribution tarball after tagging"
	@echo "make patch     - creates a patch file after tagging"
	@echo "================ maintainers only =========================="
	@echo "make lsm       - send off to the Linux Software Map"
	@echo "make ftp-local - upload tarball (good test)"
	@echo "make ftp-sunsite      - upload to sunsite"
	@echo "make ftp-sourceforge  - upload to sourceforge"
	@echo "make ftp       - upload to both sunsite & sourceforge"
	@echo ""

