.PHONY: doc pkgdoc initdir install install-testsiteconfig \
	install-testsiteconfig-1 install-testmodulerc install-testmodulerc-1 \
	install-testinitrc install-testetcrc install-testmodspath \
	install-testmodspath-empty uninstall-testconfig uninstall dist dist-tar \
	dist-gzip dist-bzip2 dist-win srpm rpm clean distclean test-deps test \
	testinstall testsyntax

# download command and its options
WGET := wget --retry-connrefused --waitretry=20 --timeout=20 --tries=3

# definitions for code coverage
NAGELFAR_DLSRC1 := http://downloads.sourceforge.net/nagelfar/
NAGELFAR_RELEASE := nagelfar131
NAGELFAR_DIST := $(NAGELFAR_RELEASE).tar.gz
NAGELFAR_DISTSUM := fbf79ab1a1d85349600f2502a3353bf4
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
ICDIFF_DLSRC := https://raw.githubusercontent.com/jeffkaufman/icdiff/release-1.9.5/
ICDIFF_CHECKSUM := fd5825ede4c2853ba1747a8931b077c1

# compatibility version-related files
COMPAT_DIR := compat

# source definitions shared across the Makefiles of this project
ifneq ($(wildcard Makefile.inc),Makefile.inc)
  $(error Makefile.inc is missing, please run './configure')
endif
include Makefile.inc

INSTALL_PREREQ := modulecmd.tcl ChangeLog README script/add.modules \
	script/createmodule.py script/modulecmd
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

# define rule prereq when target need to be rebuilt when git repository change
ifeq ($(wildcard .git),.git)
GIT_REFRESH_PREREQ := .git/index
else
GIT_REFRESH_PREREQ := 
endif

all: initdir $(INSTALL_PREREQ)

# skip doc build if no sphinx-build
ifneq ($(builddoc),n)
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

else
# load raw version information
include version.inc.in

# build short date from full date found in raw data
ifeq ($(MODULES_BUILD_FDATE),$$Format:%ci$$)
MODULES_BUILD_DATE := $(shell date '+%Y-%m-%d')
else
MODULES_BUILD_DATE := $(firstword $(MODULES_BUILD_FDATE))
endif

# set a recognizable build number if one found in version.inc.in is raw data
ifeq ($(MODULES_BUILD_HASH),$$Format:%h$$)
MODULES_BUILD := +XX-gffffffff
# or compute it from these information as if working from git repository
else
comma := ,
MODULES_BUILD_REFS := $(subst $(comma),,$(MODULES_BUILD_REFS))

ifeq ($(filter v$(MODULES_RELEASE),$(MODULES_BUILD_REFS)),v$(MODULES_RELEASE))
MODULES_BUILD :=
else ifeq ($(filter master,$(MODULES_BUILD_REFS)),master)
MODULES_BUILD := +XX-g$(MODULES_BUILD_HASH)
else ifeq ($(MODULES_BUILD_REFS),%D)
MODULES_BUILD := +XX-g$(MODULES_BUILD_HASH)
else
MODULES_BUILD := +$(lastword $(MODULES_BUILD_REFS))-XX-g$(MODULES_BUILD_HASH)
endif
endif
endif

# determine RPM release
# use last release if we currently sat on tag, append build number to it elsewhere
MODULES_LAST_RPM_VERSREL := $(shell sed -n '/^%changelog/ {n;s/^\*.* - //p;q;}' \
	contrib/rpm/environment-modules.spec.in)
MODULES_LAST_RPM_RELEASE := $(lastword $(subst -, ,$(MODULES_LAST_RPM_VERSREL)))
MODULES_RPM_RELEASE := $(MODULES_LAST_RPM_RELEASE)$(subst +,.,$(subst -,.,$(MODULES_BUILD)))

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

ifeq ($(ml),y)
  setml := 1
else
  setml := 0
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
	-e 's|@PYTHON@|$(PYTHON)|g' \
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
	-e 's|@ml@|$(setml)|g' \
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

DIST_PREFIX := modules-$(MODULES_RELEASE)$(MODULES_BUILD)
DIST_WIN_PREFIX := $(DIST_PREFIX)-win

# avoid shared definitions to be rebuilt by make
Makefile.inc: ;

version.inc: version.inc.in $(GIT_REFRESH_PREREQ)
	$(translate-in-script)

# source version definitions shared across the Makefiles of this project
include version.inc

contrib/rpm/environment-modules.spec: contrib/rpm/environment-modules.spec.in $(GIT_REFRESH_PREREQ)
	$(translate-in-script)

modulecmd.tcl: modulecmd.tcl.in version.inc
	$(translate-in-script)
	chmod +x $@

# generate an empty changelog file if not working from git repository
ifeq ($(wildcard .git),.git)
ChangeLog: script/gitlog2changelog.py
	script/gitlog2changelog.py
else
ChangeLog:
	echo "Please refer to the NEWS document to learn about main changes" >$@
endif

README:
	sed -e '1,9d' $@.md > $@

script/add.modules: script/add.modules.in
	$(translate-in-script)
	chmod +x $@

script/createmodule.py: script/createmodule.py.in
	$(translate-in-script)
	chmod +x $@

script/gitlog2changelog.py: script/gitlog2changelog.py.in
	$(translate-in-script)
	chmod +x $@

script/modulecmd: script/modulecmd.in
	$(translate-in-script)
	chmod +x $@

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
	cp script/envml $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/envml
	cp script/add.modules $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/add.modules
	cp script/createmodule.py $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/createmodule.py
	cp script/modulecmd $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/modulecmd
	cp script/mkroot $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/mkroot
ifeq ($(windowssupport),y)
	cp script/module.cmd $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/module.cmd
	cp script/ml.cmd $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/ml.cmd
	cp script/envml.cmd $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/envml.cmd
endif
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
ifneq ($(builddoc),n)
	$(MAKE) -C doc install DESTDIR=$(DESTDIR)
else
	@echo
	@echo "WARNING: Documentation not built nor installed" >&2
endif
	@echo
	@echo "NOTICE: Modules installation is complete." >&2
	@echo "        Please read the 'Configuration' section in INSTALL guide to learn" >&2
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
	rm -f $(DESTDIR)$(bindir)/createmodule.py
	rm -f $(DESTDIR)$(bindir)/modulecmd
	rm -f $(DESTDIR)$(bindir)/mkroot
ifeq ($(windowssupport),y)
	rm -f $(DESTDIR)$(bindir)/module.cmd
	rm -f $(DESTDIR)$(bindir)/ml.cmd
	rm -f $(DESTDIR)$(bindir)/envml.cmd
endif
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
ifeq ($(builddoc),n)
	rmdir $(DESTDIR)$(docdir)
endif
endif
	$(MAKE) -C init uninstall DESTDIR=$(DESTDIR)
ifneq ($(builddoc),n)
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
dist-tar: ChangeLog contrib/rpm/environment-modules.spec pkgdoc
	git archive --prefix=$(DIST_PREFIX)/ --worktree-attributes \
		-o $(DIST_PREFIX).tar HEAD
	tar -rf $(DIST_PREFIX).tar --transform 's,^,$(DIST_PREFIX)/,' \
		lib/configure lib/config.h.in ChangeLog doc/build/MIGRATING.txt \
		doc/build/diff_v3_v4.txt doc/build/INSTALL.txt doc/build/INSTALL-win.txt \
		doc/build/NEWS.txt doc/build/CONTRIBUTING.txt doc/build/module.1.in \
		doc/build/ml.1 doc/build/modulefile.4 contrib/rpm/environment-modules.spec
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

# dist zip ball for Windows platform with all pre-generated relevant files
dist-win: modulecmd.tcl ChangeLog README pkgdoc
	mkdir $(DIST_WIN_PREFIX)
	mkdir $(DIST_WIN_PREFIX)/libexec
	cp modulecmd.tcl $(DIST_WIN_PREFIX)/libexec/
	mkdir $(DIST_WIN_PREFIX)/bin
	cp script/module.cmd $(DIST_WIN_PREFIX)/bin/
	cp script/ml.cmd $(DIST_WIN_PREFIX)/bin/
	cp script/envml.cmd $(DIST_WIN_PREFIX)/bin/
	mkdir $(DIST_WIN_PREFIX)/doc
	cp COPYING.GPLv2 $(DIST_WIN_PREFIX)/doc/
	cp ChangeLog $(DIST_WIN_PREFIX)/doc/
	cp README $(DIST_WIN_PREFIX)/doc/
	cp doc/build/MIGRATING.txt $(DIST_WIN_PREFIX)/doc/
	cp doc/build/INSTALL-win.txt $(DIST_WIN_PREFIX)/doc/
	cp doc/build/NEWS.txt $(DIST_WIN_PREFIX)/doc/
	cp doc/build/CONTRIBUTING.txt $(DIST_WIN_PREFIX)/doc/
	cp doc/build/module.txt $(DIST_WIN_PREFIX)/doc/
	cp doc/build/modulefile.txt $(DIST_WIN_PREFIX)/doc/
	$(MAKE) -C init dist-win DIST_WIN_PREFIX=../$(DIST_WIN_PREFIX)
	cp script/INSTALL.bat $(DIST_WIN_PREFIX)/
	cp script/UNINSTALL.bat $(DIST_WIN_PREFIX)/
	cp script/TESTINSTALL.bat $(DIST_WIN_PREFIX)/
	zip -r $(DIST_WIN_PREFIX).zip $(DIST_WIN_PREFIX)
	rm -rf $(DIST_WIN_PREFIX)

srpm: dist-bzip2
	rpmbuild -ts $(DIST_PREFIX).tar.bz2

rpm: dist-bzip2
	rpmbuild -tb $(DIST_PREFIX).tar.bz2

clean:
	rm -f *.log *.sum
	rm -f $(MODULECMDTEST)_i $(MODULECMDTEST)_log $(MODULECMDTEST)_m
	rm -rf coverage
# do not clean generated docs if not in git repository
ifeq ($(wildcard .git),.git)
	rm -f ChangeLog
endif
	rm -f README
	rm -f modulecmd.tcl
	rm -f $(MODULECMDTEST)
	rm -f script/add.modules
	rm -f script/createmodule.py
	rm -f script/gitlog2changelog.py
	rm -f script/modulecmd
	rm -f testsuite/example/.modulespath testsuite/example/modulerc testsuite/example/modulerc-1 testsuite/example/initrc
	rm -f modules-*.tar modules-*.tar.gz modules-*.tar.bz2
	rm -rf modules-*-win/
	rm -f modules-*-win.zip
	rm -f environment-modules-*.srpm environment-modules-*.rpm
	$(MAKE) -C init clean
ifneq ($(builddoc),n)
	$(MAKE) -C doc clean
endif
	rm -f version.inc
	rm -f contrib/rpm/environment-modules.spec
ifneq ($(wildcard $(COMPAT_DIR)/Makefile),)
	$(MAKE) -C $(COMPAT_DIR) clean
endif
ifneq ($(wildcard lib/Makefile),)
	$(MAKE) -C lib clean
endif

distclean: clean
	rm -f Makefile.inc
	rm -f site.exp
	rm -f icdiff .noicdiff
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

# specific target to build test dependencies
test-deps: $(TEST_PREREQ)

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
	$(WGET) $(ICDIFF_DLSRC)$@ || true
	echo "$(ICDIFF_CHECKSUM)  $@" | md5sum --status -c - || \
		md5 -c $(ICDIFF_CHECKSUM) $@
	chmod +x $@

# install old Tcl interpreter (for code coverage purpose)
tclsh83:
	$(WGET) $(TCL_DLSRC)$(TCL_DIST83) || true
	echo "$(TCL_DISTSUM83)  $(TCL_DIST83)" | md5sum --status -c - || \
		md5 -c $(TCL_DISTSUM83) $@ || (rm -f $(TCL_DIST83) && false)
	tar xzf $(TCL_DIST83)
	cd $(TCL_RELEASE83)/unix && bash configure --disable-shared && make
	echo '#!/bin/bash' >$@
	echo 'exec $(TCLSH83) $${@}' >>$@
	chmod +x $@
	rm $(TCL_DIST83)

# install code coverage tool
# download from alt. source if correct tarball not retrieved from primary location
$(NAGELFAR):
	$(WGET) $(NAGELFAR_DLSRC1)$(NAGELFAR_DIST) || true
	echo "$(NAGELFAR_DISTSUM)  $(NAGELFAR_DIST)" | md5sum --status -c - || \
		(rm -f $(NAGELFAR_DIST) && false)
	tar xzf $(NAGELFAR_DIST)
	rm $(NAGELFAR_DIST)

testsyntax: $(MODULECMDTEST) $(NAGELFAR)
	$(NAGELFAR) -len 78 $<
