.PHONY: doc pkgdoc initdir install install-testsiteconfig \
	install-testsiteconfig-1 install-testmodulerc install-testmodulerc-1 \
	install-testinitrc install-testetcrc install-testmodspath \
	install-testmodspath-empty uninstall-testconfig uninstall dist dist-tar \
	dist-gzip dist-bzip2 srpm clean distclean test testinstall testsyntax

# definitions for code coverage
NAGELFAR_DLSRC1 := http://downloads.sourceforge.net/nagelfar/
NAGELFAR_DLSRC2 := https://ftp.openbsd.org/pub/OpenBSD/distfiles/
NAGELFAR_RELEASE := nagelfar125
NAGELFAR_DIST := $(NAGELFAR_RELEASE).tar.gz
NAGELFAR_DISTSUM := 707e3c305437dce1f14103f0bd058fc9
NAGELFAR := $(NAGELFAR_RELEASE)/nagelfar.tcl

# definition for old Tcl interpreter for coverage testing
TCL_DLSRC := http://downloads.sourceforge.net/tcl/
TCL_RELEASE83 := tcl8.3.5
TCL_DIST83 := $(TCL_RELEASE83)-src.tar.gz
TCL_DISTSUM83 := 5cb79f8b90cf1322cb1286b9fe67f7a2
TCLSH83 := $(TCL_RELEASE83)/unix/tclsh

# specific modulecmd script for test
MODULECMDTEST := modulecmd-test.tcl

# definitions for enhanced diff tool (to review test results)
ICDIFF_DLSRC := https://raw.githubusercontent.com/jeffkaufman/icdiff/release-1.9.2/
ICDIFF_CHECKSUM := 2bf052d9dfb0a46af581fc390c47774a

# compatibility version-related files
COMPAT_DIR := compat

# source definitions shared across the Makefiles of this project
ifneq ($(wildcard Makefile.inc),Makefile.inc)
  $(error Makefile.inc is missing, please run './configure')
endif
include Makefile.inc

INSTALL_PREREQ := modulecmd.tcl ChangeLog README contrib/scripts/add.modules \
	contrib/scripts/modulecmd
TEST_PREREQ := $(MODULECMDTEST)

ifeq ($(compatversion),y)
INSTALL_PREREQ += $(COMPAT_DIR)/modulecmd$(EXEEXT) $(COMPAT_DIR)/ChangeLog
ifeq ($(wildcard $(COMPAT_DIR)),$(COMPAT_DIR))
TEST_PREREQ += $(COMPAT_DIR)/modulecmd
endif
endif

ifeq ($(libtclenvmodules),y)
INSTALL_PREREQ += lib/libtclenvmodules$(SHLIB_SUFFIX)
TEST_PREREQ += lib/libtclenvmodules$(SHLIB_SUFFIX)
ifeq ($(COVERAGE),y)
TEST_PREREQ += lib/libtestutil-closedir$(SHLIB_SUFFIX)
endif
endif

ifeq ($(COVERAGE),y)
TEST_PREREQ += $(NAGELFAR)
endif

# install old Tcl interpreters to test coverage
ifeq ($(COVERAGE_OLDTCL),y)
TEST_PREREQ += tclsh83
endif

all: initdir $(INSTALL_PREREQ)

# skip doc build if no sphinx-build
ifeq ($(builddoc),y)
all: pkgdoc
endif

initdir: version.inc
	$(MAKE) -C init all

pkgdoc: version.inc
	$(MAKE) -C doc man txt

doc: version.inc
	$(MAKE) -C doc all

# build version.inc shared definitions from git repository info
ifeq ($(wildcard .git) $(wildcard version.inc.in),.git version.inc.in)
GIT_CURRENT_TAG := $(shell git describe --tags --abbrev=0)
GIT_CURRENT_DESC := $(shell git describe --tags)
GIT_CURRENT_BRANCH := $(shell git rev-parse --abbrev-ref HEAD)

MODULES_RELEASE := $(subst v,,$(GIT_CURRENT_TAG))
MODULES_BUILD_DATE := $(shell git log -1 --format=%cd --date=short)
ifeq ($(GIT_CURRENT_TAG),$(GIT_CURRENT_DESC))
MODULES_BUILD :=
else ifeq ($(GIT_CURRENT_BRANCH),master)
MODULES_BUILD := +$(subst $(GIT_CURRENT_TAG)-,,$(GIT_CURRENT_DESC))
else
MODULES_BUILD := +$(GIT_CURRENT_BRANCH)$(subst $(GIT_CURRENT_TAG),,$(GIT_CURRENT_DESC))
endif

# determine RPM release
# use last release if we currently sat on tag, append build number to it elsewhere
MODULES_LAST_RPM_VERSREL := $(shell sed -n '/^%changelog/ {n;s/^\*.* - //p;q;}' \
	contrib/rpm/environment-modules.spec.in)
MODULES_LAST_RPM_RELEASE := $(lastword $(subst -, ,$(MODULES_LAST_RPM_VERSREL)))
MODULES_RPM_RELEASE := $(MODULES_LAST_RPM_RELEASE)$(subst +,.,$(subst -,.,$(MODULES_BUILD)))

else
# source version definitions shared across the Makefiles of this project
ifneq ($(wildcard version.inc),version.inc)
  $(error version.inc is missing)
endif
include version.inc
endif

# define init configs location
ifeq ($(initconfin),etcdir)
  modulespath := $(etcdir)/modulespath
  initrc := $(etcdir)/initrc
else
  modulespath := $(initdir)/.modulespath
  initrc := $(initdir)/modulerc
endif

# comment entries if feature not enabled
ifeq ($(versioning),y)
  setversioning :=
  setnotversioning := \#
else
  setversioning := \#
  setnotversioning :=
endif

ifeq ($(silentshdbgsupport),y)
  setsilentshdbgsupport :=
else
  setsilentshdbgsupport := \#
endif

ifeq ($(quarantinesupport),y)
  setquarantinesupport :=
  setnotquarantinesupport := \#
else
  setquarantinesupport := \#
  setnotquarantinesupport :=
endif

ifeq ($(libtclenvmodules),y)
  setlibtclenvmodules :=
else
  setlibtclenvmodules := \#
endif

ifneq ($(pageropts),)
  pagercmd := $(pager) $(pageropts)
else
  pagercmd := $(pager)
endif

ifeq ($(color),y)
  setcolor := auto
else
  setcolor := never
endif

ifeq ($(autohandling),y)
  setautohandling := 1
else
  setautohandling := 0
endif

ifeq ($(availindepth),y)
  setavailindepth := 1
else
  setavailindepth := 0
endif

ifeq ($(implicitdefault),y)
  setimplicitdefault := 1
else
  setimplicitdefault := 0
endif

ifeq ($(extendeddefault),y)
  setextendeddefault := 1
else
  setextendeddefault := 0
endif

ifeq ($(advversspec),y)
  setadvversspec := 1
else
  setadvversspec := 0
endif

ifeq ($(setshellstartup),y)
  setsetshellstartup := 1
else
  setsetshellstartup := 0
endif

ifeq ($(wa277),y)
  setwa277 := 1
else
  setwa277 := 0
endif

define translate-in-script
sed -e 's|@prefix@|$(prefix)|g' \
	-e 's|@baseprefix@|$(baseprefix)|g' \
	-e 's|@libdir@|$(libdir)|g' \
	-e 's|@libexecdir@|$(libexecdir)|g' \
	-e 's|@initdir@|$(initdir)|g' \
	-e 's|@etcdir@|$(etcdir)|g' \
	-e 's|@modulefilesdir@|$(modulefilesdir)|g' \
	-e 's|@moduleshome@|$(moduleshome)|g' \
	-e 's|@VERSION@|$(VERSION)|g' \
	-e 's|@TCLSHDIR@/tclsh|$(TCLSH)|g' \
	-e 's|@TCLSH@|$(TCLSH)|g' \
	-e 's|@pagercmd@|$(pagercmd)|g' \
	-e 's|@verbosity@|$(verbosity)|g' \
	-e 's|@color@|$(setcolor)|g' \
	-e 's|@darkbgcolors@|$(darkbgcolors)|g' \
	-e 's|@lightbgcolors@|$(lightbgcolors)|g' \
	-e 's|@termbg@|$(termbg)|g' \
	-e 's|@lockedconfigs@|$(lockedconfigs)|g' \
	-e 's|@unloadmatchorder@|$(unloadmatchorder)|g' \
	-e 's|@implicitdefault@|$(setimplicitdefault)|g' \
	-e 's|@extendeddefault@|$(setextendeddefault)|g' \
	-e 's|@advversspec@|$(setadvversspec)|g' \
	-e 's|@searchmatch@|$(searchmatch)|g' \
	-e 's|@wa277@|$(setwa277)|g' \
	-e 's|@icase@|$(icase)|g' \
	-e 's|@autohandling@|$(setautohandling)|g' \
	-e 's|@availindepth@|$(setavailindepth)|g' \
	-e 's|@silentshdbgsupport@|$(setsilentshdbgsupport)|g' \
	-e 's|@setshellstartup@|$(setsetshellstartup)|g' \
	-e 's|@quarantinesupport@|$(setquarantinesupport)|g' \
	-e 's|@notquarantinesupport@|$(setnotquarantinesupport)|g' \
	-e 's|@libtclenvmodules@|$(setlibtclenvmodules)|g' \
	-e 's|@SHLIB_SUFFIX@|$(SHLIB_SUFFIX)|g' \
	-e 's|@VERSIONING@|$(setversioning)|g' \
	-e 's|@NOTVERSIONING@|$(setnotversioning)|g' \
	-e 's|@MODULES_RELEASE@|$(MODULES_RELEASE)|g' \
	-e 's|@MODULES_BUILD@|$(MODULES_BUILD)|g' \
	-e 's|@MODULES_RPM_RELEASE@|$(MODULES_RPM_RELEASE)|g' \
	-e 's|@MODULES_BUILD_DATE@|$(MODULES_BUILD_DATE)|g' $< > $@
endef

ifeq ($(wildcard .git) $(wildcard version.inc.in),.git version.inc.in)
# rebuild if git repository changed
version.inc: version.inc.in .git/index
	$(translate-in-script)

contrib/rpm/environment-modules.spec: contrib/rpm/environment-modules.spec.in .git/index
	$(translate-in-script)

else
# avoid shared definitions to be rebuilt by make
version.inc: ;

# do not rebuild spec file if not in git repository
contrib/rpm/environment-modules.spec: ;
endif

DIST_PREFIX := modules-$(MODULES_RELEASE)$(MODULES_BUILD)

# avoid shared definitions to be rebuilt by make
Makefile.inc: ;

modulecmd.tcl: modulecmd.tcl.in version.inc
	$(translate-in-script)
	chmod +x $@

ChangeLog:
	contrib/gitlog2changelog.py

README:
	sed -e '/^\[\!\[.*\].*/d' $@.md > $@

contrib/mtreview: contrib/mtreview.in version.inc
	$(translate-in-script)
	chmod +x $@

contrib/mb: contrib/mb.in version.inc
	$(translate-in-script)
	chmod +x $@

contrib/scripts/add.modules: contrib/scripts/add.modules.in
	$(translate-in-script)

contrib/scripts/modulecmd: contrib/scripts/modulecmd.in
	$(translate-in-script)

# compatibility version-related rules
$(COMPAT_DIR)/modulecmd$(EXEEXT) $(COMPAT_DIR)/ChangeLog:
	$(MAKE) -C $(COMPAT_DIR) $(@F)

# Tcl extension library-related rules
lib/libtclenvmodules$(SHLIB_SUFFIX):
	$(MAKE) -C lib $(@F)

lib/libtestutil-closedir$(SHLIB_SUFFIX):
	$(MAKE) -C lib $(@F)

# example configs for test rules
testsuite/example/.modulespath: testsuite/example/.modulespath.in
	$(translate-in-script)

testsuite/example/modulerc: testsuite/example/modulerc.in
	$(translate-in-script)

testsuite/example/modulerc-1: testsuite/example/modulerc-1.in
	$(translate-in-script)

testsuite/example/initrc: testsuite/example/initrc.in
	$(translate-in-script)

install-testsiteconfig: testsuite/example/siteconfig.tcl
	$(MAKE) -C init install-testconfig DESTDIR=$(DESTDIR)
	cp $^ $(DESTDIR)$(etcdir)/

install-testsiteconfig-1: testsuite/example/siteconfig.tcl-1
	$(MAKE) -C init install-testconfig DESTDIR=$(DESTDIR)
	cp $^ $(DESTDIR)$(etcdir)/siteconfig.tcl

install-testmodulerc: testsuite/example/modulerc
	$(MAKE) -C init install-testconfig DESTDIR=$(DESTDIR)
	cp $^ $(DESTDIR)$(initrc)

install-testmodulerc-1: testsuite/example/modulerc-1
	$(MAKE) -C init install-testconfig DESTDIR=$(DESTDIR)
	cp $^ $(DESTDIR)$(initrc)

install-testinitrc: testsuite/example/initrc
	$(MAKE) -C init install-testconfig DESTDIR=$(DESTDIR)
	cp $^ $(DESTDIR)$(initrc)

install-testetcrc: testsuite/etc/empty
	$(MAKE) -C init install-testconfig DESTDIR=$(DESTDIR)
	cp $^ $(DESTDIR)$(etcdir)/rc

install-testmodspath: testsuite/example/.modulespath
	$(MAKE) -C init install-testconfig DESTDIR=$(DESTDIR)
	cp $^ $(DESTDIR)$(modulespath)

install-testmodspath-empty: testsuite/example/.modulespath-empty
	$(MAKE) -C init install-testconfig DESTDIR=$(DESTDIR)
	cp $^ $(DESTDIR)$(modulespath)

uninstall-testconfig:
	rm -f $(DESTDIR)$(etcdir)/rc
	rm -f $(DESTDIR)$(etcdir)/siteconfig.tcl
	rm -f $(DESTDIR)$(initrc)
	rm -f $(DESTDIR)$(modulespath)
	$(MAKE) -C init uninstall-testconfig DESTDIR=$(DESTDIR)

install: $(INSTALL_PREREQ)
	mkdir -p $(DESTDIR)$(libexecdir)
	mkdir -p $(DESTDIR)$(bindir)
	mkdir -p $(DESTDIR)$(etcdir)
	cp modulecmd.tcl $(DESTDIR)$(libexecdir)/
	chmod +x $(DESTDIR)$(libexecdir)/modulecmd.tcl
ifeq ($(compatversion),y)
	cp $(COMPAT_DIR)/modulecmd$(EXEEXT) $(DESTDIR)$(libexecdir)/modulecmd-compat$(EXEEXT)
	chmod +x $(DESTDIR)$(libexecdir)/modulecmd-compat$(EXEEXT)
endif
ifeq ($(libtclenvmodules),y)
	mkdir -p $(DESTDIR)$(libdir)
	cp lib/libtclenvmodules$(SHLIB_SUFFIX) $(DESTDIR)$(libdir)/libtclenvmodules$(SHLIB_SUFFIX)
	chmod +x $(DESTDIR)$(libdir)/libtclenvmodules$(SHLIB_SUFFIX)
endif
	cp contrib/envml $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/envml
	cp contrib/scripts/add.modules $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/add.modules
	cp contrib/scripts/modulecmd $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/modulecmd
	cp contrib/scripts/mkroot $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/mkroot
ifneq ($(wildcard $(DESTDIR)$(etcdir)/siteconfig.tcl),$(DESTDIR)$(etcdir)/siteconfig.tcl)
	cp siteconfig.tcl $(DESTDIR)$(etcdir)/siteconfig.tcl
endif
ifeq ($(docinstall),y)
	mkdir -p $(DESTDIR)$(docdir)
	cp COPYING.GPLv2 $(DESTDIR)$(docdir)/
	cp ChangeLog $(DESTDIR)$(docdir)/
	cp README $(DESTDIR)$(docdir)/
ifeq ($(compatversion),y)
	cp $(COMPAT_DIR)/ChangeLog $(DESTDIR)$(docdir)/ChangeLog-compat
	cp $(COMPAT_DIR)/NEWS $(DESTDIR)$(docdir)/NEWS-compat
endif
endif
ifeq ($(vimaddons),y)
	mkdir -p $(DESTDIR)$(vimdatadir)/ftdetect
	mkdir -p $(DESTDIR)$(vimdatadir)/ftplugin
	mkdir -p $(DESTDIR)$(vimdatadir)/syntax
	cp  contrib/vim/ftdetect/modulefile.vim  $(DESTDIR)$(vimdatadir)/ftdetect
	cp  contrib/vim/ftplugin/modulefile.vim  $(DESTDIR)$(vimdatadir)/ftplugin
	cp  contrib/vim/syntax/modulefile.vim    $(DESTDIR)$(vimdatadir)/syntax
endif
	$(MAKE) -C init install DESTDIR=$(DESTDIR)
ifeq ($(builddoc),y)
	$(MAKE) -C doc install DESTDIR=$(DESTDIR)
else
	@echo
	@echo "WARNING: Documentation not built nor installed"
endif
	@echo
	@echo "NOTICE: Modules installation is complete."
	@echo "        Please read the 'Configuration' section in INSTALL.txt to learn"
	@echo "        how to adapt your installation and make it fit your needs." >&2
	@echo

uninstall:
	rm -f $(DESTDIR)$(libexecdir)/modulecmd.tcl
ifeq ($(compatversion),y)
	rm -f $(DESTDIR)$(libexecdir)/modulecmd-compat$(EXEEXT)
endif
ifeq ($(libtclenvmodules),y)
	rm -f $(DESTDIR)$(libdir)/libtclenvmodules$(SHLIB_SUFFIX)
endif
	rm -f $(DESTDIR)$(bindir)/envml
	rm -f $(DESTDIR)$(bindir)/add.modules
	rm -f $(DESTDIR)$(bindir)/modulecmd
	rm -f $(DESTDIR)$(bindir)/mkroot
ifeq ($(vimaddons),y)
	rm -f $(DESTDIR)$(vimdatadir)/ftdetect/modulefile.vim
	rm -f $(DESTDIR)$(vimdatadir)/ftplugin/modulefile.vim
	rm -f $(DESTDIR)$(vimdatadir)/syntax/modulefile.vim
	-rmdir $(DESTDIR)$(vimdatadir)/ftdetect
	-rmdir $(DESTDIR)$(vimdatadir)/ftplugin
	-rmdir $(DESTDIR)$(vimdatadir)/syntax
	-rmdir -p $(DESTDIR)$(vimdatadir)
endif
ifeq ($(docinstall),y)
	rm -f $(addprefix $(DESTDIR)$(docdir)/,ChangeLog README COPYING.GPLv2)
ifeq ($(compatversion),y)
	rm -f $(addprefix $(DESTDIR)$(docdir)/,ChangeLog-compat NEWS-compat)
endif
ifneq ($(builddoc),y)
	rmdir $(DESTDIR)$(docdir)
endif
endif
	$(MAKE) -C init uninstall DESTDIR=$(DESTDIR)
ifeq ($(builddoc),y)
	$(MAKE) -C doc uninstall DESTDIR=$(DESTDIR)
endif
	rmdir $(DESTDIR)$(libexecdir)
ifeq ($(libtclenvmodules),y)
	rmdir $(DESTDIR)$(libdir)
endif
	rmdir $(DESTDIR)$(bindir)
	rmdir $(DESTDIR)$(datarootdir)
	$(RMDIR_IGN_NON_EMPTY) $(DESTDIR)$(prefix) || true

# include pre-generated documents not to require documentation build
# tools when installing from dist tarball
dist-tar: ChangeLog README version.inc contrib/rpm/environment-modules.spec pkgdoc
	git archive --prefix=$(DIST_PREFIX)/ --worktree-attributes \
		-o $(DIST_PREFIX).tar HEAD
	cp doc/build/MIGRATING.txt  doc/build/INSTALL.txt doc/build/NEWS.txt \
		doc/build/CONTRIBUTING.txt ./
	tar -rf $(DIST_PREFIX).tar --transform 's,^,$(DIST_PREFIX)/,' \
		lib/configure lib/config.h.in ChangeLog README MIGRATING.txt INSTALL.txt \
		NEWS.txt CONTRIBUTING.txt version.inc doc/build/MIGRATING.txt \
		doc/build/diff_v3_v4.txt doc/build/INSTALL.txt doc/build/NEWS.txt \
		doc/build/CONTRIBUTING.txt doc/build/module.1.in doc/build/modulefile.4 \
		contrib/rpm/environment-modules.spec
ifeq ($(compatversion) $(wildcard $(COMPAT_DIR)),y $(COMPAT_DIR))
	$(MAKE) -C $(COMPAT_DIR) distdir
	mv $(COMPAT_DIR)/modules-* compatdist
	tar -cf compatdist.tar --transform 's,^compatdist,$(DIST_PREFIX)/compat,' compatdist
	tar --concatenate -f $(DIST_PREFIX).tar compatdist.tar
	rm -rf compatdist
	rm compatdist.tar
endif

dist-gzip: dist-tar
	gzip -f -9 $(DIST_PREFIX).tar

dist-bzip2: dist-tar
	bzip2 -f $(DIST_PREFIX).tar

dist: dist-gzip

srpm: dist-bzip2
	rpmbuild -ts $(DIST_PREFIX).tar.bz2

clean:
	rm -f *.log *.sum
	rm -f $(MODULECMDTEST)_i $(MODULECMDTEST)_log $(MODULECMDTEST)_m
	rm -rf coverage
ifeq ($(wildcard .git) $(wildcard contrib/gitlog2changelog.py),.git contrib/gitlog2changelog.py)
	rm -f ChangeLog
endif
ifeq ($(wildcard .git) $(wildcard README.md),.git README.md)
	rm -f README
endif
ifeq ($(wildcard .git) $(wildcard MIGRATING.rst),.git MIGRATING.rst)
	rm -f MIGRATING.txt
endif
ifeq ($(wildcard .git) $(wildcard INSTALL.rst),.git INSTALL.rst)
	rm -f INSTALL.txt
endif
ifeq ($(wildcard .git) $(wildcard NEWS.rst),.git NEWS.rst)
	rm -f NEWS.txt
endif
ifeq ($(wildcard .git) $(wildcard CONTRIBUTING.rst),.git CONTRIBUTING.rst)
	rm -f CONTRIBUTING.txt
endif
	rm -f modulecmd.tcl
	rm -f contrib/mtreview
	rm -f contrib/mb
	rm -f $(MODULECMDTEST)
	rm -f contrib/scripts/add.modules
	rm -f contrib/scripts/modulecmd
	rm -f testsuite/example/.modulespath testsuite/example/modulerc testsuite/example/modulerc-1 testsuite/example/initrc
	rm -f modules-*.tar modules-*.tar.gz modules-*.tar.bz2
	rm -f modules-*.srpm
	$(MAKE) -C init clean
ifeq ($(builddoc),y)
	$(MAKE) -C doc clean
endif
ifeq ($(wildcard .git) $(wildcard version.inc.in),.git version.inc.in)
	rm -f version.inc
	rm -f contrib/rpm/environment-modules.spec
endif
ifneq ($(wildcard $(COMPAT_DIR)/Makefile),)
	$(MAKE) -C $(COMPAT_DIR) clean
endif
ifneq ($(wildcard lib/Makefile),)
	$(MAKE) -C lib clean
endif

distclean: clean
	rm -f Makefile.inc
	rm -f site.exp
	rm -f icdiff
	rm -rf $(NAGELFAR_RELEASE)
	rm -rf $(TCL_RELEASE83)
	rm -f tclsh83
ifeq ($(wildcard .git) $(wildcard $(COMPAT_DIR)),.git $(COMPAT_DIR))
	rm -rf $(COMPAT_DIR)
ifeq ($(gitworktree),y)
	git worktree prune
endif
endif
ifneq ($(wildcard lib/Makefile),)
	$(MAKE) -C lib distclean
endif

# prepare for code coverage run
ifeq ($(COVERAGE),y)
$(MODULECMDTEST): $(NAGELFAR)
endif

# make specific modulecmd script for test to check built extension lib
# if coverage asked, instrument script and clear previous coverage log
$(MODULECMDTEST): modulecmd.tcl
	sed -e 's|$(libdir)|lib|' $< > $@
ifeq ($(COVERAGE),y)
	rm -f $(MODULECMDTEST)_log
	$(NAGELFAR) -instrument $@
endif

# if coverage enabled, run tests on instrumented file to create coverage log
ifeq ($(COVERAGE),y)
export MODULECMD = $(MODULECMDTEST)_i
endif

# if coverage enabled create markup file for better read coverage result
test: $(TEST_PREREQ)
ifeq ($(compatversion) $(wildcard $(COMPAT_DIR)),y $(COMPAT_DIR))
	$(MAKE) -C $(COMPAT_DIR) test
endif
	TCLSH=$(TCLSH); export TCLSH; \
	OBJDIR=`pwd -P`; export OBJDIR; \
	TESTSUITEDIR=`cd testsuite;pwd -P`; export TESTSUITEDIR; \
	runtest --srcdir $$TESTSUITEDIR --objdir $$OBJDIR $(RUNTESTFLAGS) --tool modules $(RUNTESTFILES)
ifeq ($(COVERAGE),y)
	$(NAGELFAR) -markup $(MODULECMDTEST)
endif

testinstall:
	OBJDIR=`pwd -P`; export OBJDIR; \
	TESTSUITEDIR=`cd testsuite;pwd -P`; export TESTSUITEDIR; \
	runtest --srcdir $$TESTSUITEDIR --objdir $$OBJDIR $(RUNTESTFLAGS) --tool install $(RUNTESTFILES)


# install enhanced diff tool (to review test results)
icdiff:
	wget $(ICDIFF_DLSRC)$@ || true
	echo "$(ICDIFF_CHECKSUM)  $@" | md5sum --status -c - || \
		md5 -c $(ICDIFF_CHECKSUM) $@
	chmod +x $@

# install old Tcl interpreter (for code coverage purpose)
tclsh83:
	wget $(TCL_DLSRC)$(TCL_DIST83) || true
	echo "$(TCL_DISTSUM83)  $(TCL_DIST83)" | md5sum --status -c - || \
		md5 -c $(TCL_DISTSUM83) $@
	tar xzf $(TCL_DIST83)
	cd $(TCL_RELEASE83)/unix && bash configure --disable-shared && make
	echo '#!/bin/bash' >$@
	echo 'exec $(TCLSH83) $${@}' >>$@
	chmod +x $@
	rm $(TCL_DIST83)

# install code coverage tool
# download from alt. source if correct tarball not retrieved from primary location
$(NAGELFAR):
	wget $(NAGELFAR_DLSRC1)$(NAGELFAR_DIST) || true
	echo "$(NAGELFAR_DISTSUM)  $(NAGELFAR_DIST)" | md5sum --status -c - || \
		wget -O $(NAGELFAR_DIST) $(NAGELFAR_DLSRC2)$(NAGELFAR_DIST)
	tar xzf $(NAGELFAR_DIST)
	rm $(NAGELFAR_DIST)

testsyntax: $(MODULECMDTEST) $(NAGELFAR)
	$(NAGELFAR) -len 78 $<
