.PHONY: doc pkgdoc initdir install install-testsiteconfig \
	install-testsiteconfig-1 install-testmodulerc install-testetcrc \
	install-testmodspath install-testmodspath-empty uninstall-testconfig \
	uninstall dist dist-tar dist-gzip dist-bzip2 srpm clean distclean test \
	testinstall instrument testcoverage

# definitions for code coverage
NAGELFAR_DLSRC1 := http://downloads.sourceforge.net/nagelfar/
NAGELFAR_DLSRC2 := https://ftp.openbsd.org/pub/OpenBSD/distfiles/
NAGELFAR_RELEASE := nagelfar125
NAGELFAR_DIST := $(NAGELFAR_RELEASE).tar.gz
NAGELFAR_DISTSUM := 707e3c305437dce1f14103f0bd058fc9
NAGELFAR := $(NAGELFAR_RELEASE)/nagelfar.tcl
COVERAGE_SRCFILE := modulecmd.tcl
COVERAGE_IFILE := $(COVERAGE_SRCFILE:.tcl=.tcl_i)
COVERAGE_LOGFILE := $(COVERAGE_SRCFILE:.tcl=.tcl_log)
COVERAGE_MFILE := $(COVERAGE_SRCFILE:.tcl=.tcl_m)

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

all: initdir modulecmd.tcl ChangeLog README \
	contrib/scripts/add.modules contrib/scripts/modulecmd
ifeq ($(compatversion),y)
all: $(COMPAT_DIR)/modulecmd$(EXEEXT) $(COMPAT_DIR)/ChangeLog
else
endif
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

ifeq ($(autohandling),y)
  setautohandling := 1
else
  setautohandling := 0
endif

define translate-in-script
sed -e 's|@prefix@|$(prefix)|g' \
	-e 's|@baseprefix@|$(baseprefix)|g' \
	-e 's|@libexecdir@|$(libexecdir)|g' \
	-e 's|@initdir@|$(initdir)|g' \
	-e 's|@etcdir@|$(etcdir)|g' \
	-e 's|@modulefilesdir@|$(modulefilesdir)|g' \
	-e 's|@VERSION@|$(VERSION)|g' \
	-e 's|@TCLSHDIR@/tclsh|$(TCLSH)|g' \
	-e 's|@TCLSH@|$(TCLSH)|g' \
	-e 's|@pager@|$(pager)|g' \
	-e 's|@pageropts@|$(pageropts)|g' \
	-e 's|@autohandling@|$(setautohandling)|g' \
	-e 's|@silentshdbgsupport@|$(setsilentshdbgsupport)|g' \
	-e 's|@quarantinesupport@|$(setquarantinesupport)|g' \
	-e 's|@notquarantinesupport@|$(setnotquarantinesupport)|g' \
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

contrib/scripts/add.modules: contrib/scripts/add.modules.in
	$(translate-in-script)

contrib/scripts/modulecmd: contrib/scripts/modulecmd.in
	$(translate-in-script)

# compatibility version-related rules
$(COMPAT_DIR)/modulecmd$(EXEEXT) $(COMPAT_DIR)/ChangeLog:
	$(MAKE) -C $(COMPAT_DIR) $(@F)

# example configs for test rules
testsuite/example/.modulespath: testsuite/example/.modulespath.in
	$(translate-in-script)

testsuite/example/modulerc: testsuite/example/modulerc.in
	$(translate-in-script)

install-testsiteconfig: testsuite/example/siteconfig.tcl
	$(MAKE) -C init install-testconfig DESTDIR=$(DESTDIR)
	cp $^ $(DESTDIR)$(etcdir)/

install-testsiteconfig-1: testsuite/example/siteconfig.tcl-1
	$(MAKE) -C init install-testconfig DESTDIR=$(DESTDIR)
	cp $^ $(DESTDIR)$(etcdir)/siteconfig.tcl

install-testmodulerc: testsuite/example/modulerc
	$(MAKE) -C init install-testconfig DESTDIR=$(DESTDIR)
	cp $^ $(DESTDIR)$(initdir)/

install-testetcrc: testsuite/etc/empty
	$(MAKE) -C init install-testconfig DESTDIR=$(DESTDIR)
	cp $^ $(DESTDIR)$(prefix)/etc/rc

install-testmodspath: testsuite/example/.modulespath
	$(MAKE) -C init install-testconfig DESTDIR=$(DESTDIR)
	cp $^ $(DESTDIR)$(initdir)/

install-testmodspath-empty: testsuite/example/.modulespath-empty
	$(MAKE) -C init install-testconfig DESTDIR=$(DESTDIR)
	cp $^ $(DESTDIR)$(initdir)/.modulespath

uninstall-testconfig:
	rm -f $(DESTDIR)$(prefix)/etc/rc
	rm -f $(DESTDIR)$(etcdir)/siteconfig.tcl
	rm -f $(DESTDIR)$(initdir)/modulerc
	rm -f $(DESTDIR)$(initdir)/.modulespath
	$(MAKE) -C init uninstall-testconfig DESTDIR=$(DESTDIR)

ifeq ($(compatversion),y)
install: modulecmd.tcl ChangeLog README contrib/scripts/add.modules \
	contrib/scripts/modulecmd $(COMPAT_DIR)/modulecmd$(EXEEXT) $(COMPAT_DIR)/ChangeLog
else
install: modulecmd.tcl ChangeLog README contrib/scripts/add.modules \
	contrib/scripts/modulecmd
endif
	mkdir -p $(DESTDIR)$(libexecdir)
	mkdir -p $(DESTDIR)$(bindir)
	cp modulecmd.tcl $(DESTDIR)$(libexecdir)/
	chmod +x $(DESTDIR)$(libexecdir)/modulecmd.tcl
ifeq ($(compatversion),y)
	cp $(COMPAT_DIR)/modulecmd$(EXEEXT) $(DESTDIR)$(libexecdir)/modulecmd-compat$(EXEEXT)
	chmod +x $(DESTDIR)$(libexecdir)/modulecmd-compat$(EXEEXT)
endif
	cp contrib/envml $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/envml
	cp contrib/scripts/add.modules $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/add.modules
	cp contrib/scripts/modulecmd $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/modulecmd
	cp contrib/scripts/mkroot $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/mkroot
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
	rm -f $(DESTDIR)$(bindir)/envml
	rm -f $(DESTDIR)$(bindir)/add.modules
	rm -f $(DESTDIR)$(bindir)/modulecmd
	rm -f $(DESTDIR)$(bindir)/mkroot
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
		ChangeLog README MIGRATING.txt INSTALL.txt NEWS.txt CONTRIBUTING.txt \
		version.inc doc/build/MIGRATING.txt doc/build/diff_v3_v4.txt \
		doc/build/INSTALL.txt doc/build/NEWS.txt doc/build/CONTRIBUTING.txt \
		doc/build/module.1.in doc/build/modulefile.4 \
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
	rm -f $(COVERAGE_IFILE) $(COVERAGE_LOGFILE) $(COVERAGE_MFILE)
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
	rm -f contrib/scripts/add.modules
	rm -f contrib/scripts/modulecmd
	rm -f testsuite/example/.modulespath testsuite/example/modulerc
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
	$(MAKE) -C compat clean
endif

distclean: clean
	rm -f Makefile.inc
	rm -f site.exp
	rm -f icdiff
	rm -rf $(NAGELFAR_RELEASE)
ifeq ($(wildcard .git) $(wildcard $(COMPAT_DIR)),.git $(COMPAT_DIR))
	rm -rf $(COMPAT_DIR)
ifeq ($(gitworktree),y)
	git worktree prune
endif
endif

ifeq ($(compatversion) $(wildcard $(COMPAT_DIR)),y $(COMPAT_DIR))
test: modulecmd.tcl $(COMPAT_DIR)/modulecmd
	$(MAKE) -C $(COMPAT_DIR) test
else
test: modulecmd.tcl
endif
	TCLSH=$(TCLSH); export TCLSH; \
	OBJDIR=`pwd -P`; export OBJDIR; \
	TESTSUITEDIR=`cd testsuite;pwd -P`; export TESTSUITEDIR; \
	runtest --srcdir $$TESTSUITEDIR --objdir $$OBJDIR $(RUNTESTFLAGS) --tool modules

testinstall:
	OBJDIR=`pwd -P`; export OBJDIR; \
	TESTSUITEDIR=`cd testsuite;pwd -P`; export TESTSUITEDIR; \
	runtest --srcdir $$TESTSUITEDIR --objdir $$OBJDIR $(RUNTESTFLAGS) --tool install


# install enhanced diff tool (to review test results)
icdiff:
	wget $(ICDIFF_DLSRC)$@ || true
	echo "$(ICDIFF_CHECKSUM)  $@" | md5sum --status -c - || \
		md5 -c $(ICDIFF_CHECKSUM) $@
	chmod +x $@

# install code coverage tool
# download from alt. source if correct tarball not retrieved from primary location
$(NAGELFAR):
	wget $(NAGELFAR_DLSRC1)$(NAGELFAR_DIST) || true
	echo "$(NAGELFAR_DISTSUM)  $(NAGELFAR_DIST)" | md5sum --status -c - || \
		wget -O $(NAGELFAR_DIST) $(NAGELFAR_DLSRC2)$(NAGELFAR_DIST)
	tar xzf $(NAGELFAR_DIST)
	rm $(NAGELFAR_DIST)

# instrument source file for code coverage
%.tcl_i: %.tcl $(NAGELFAR)
	$(NAGELFAR) -instrument $<

# prepare for code coverage run and make sure coverage log is clear
instrument: $(COVERAGE_IFILE)
	rm -f $(COVERAGE_LOGFILE)

# run tests on instrumented file to create coverage log
$(COVERAGE_LOGFILE): $(COVERAGE_IFILE)
	MODULECMD=./$< $(MAKE) test

# create markup file for better read coverage result
%.tcl_m: %.tcl_log $(NAGELFAR)
	$(NAGELFAR) -markup $*.tcl

testcoverage: instrument $(COVERAGE_MFILE)

testsyntax: $(NAGELFAR) $(COVERAGE_SRCFILE)
	$(NAGELFAR) -len 78 $(COVERAGE_SRCFILE)
