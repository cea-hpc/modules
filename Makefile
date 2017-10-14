.PHONY: doc pkgdoc www initdir install uninstall dist dist-tar dist-gzip \
	dist-bzip2 srpm clean distclean test testinstall instrument testcoverage

# definitions for code coverage
NAGELFAR_RELEASE := nagelfar125
NAGELFAR := $(NAGELFAR_RELEASE)/nagelfar.tcl
COVERAGE_SRCFILE := modulecmd.tcl
COVERAGE_IFILE := $(COVERAGE_SRCFILE:.tcl=.tcl_i)
COVERAGE_LOGFILE := $(COVERAGE_SRCFILE:.tcl=.tcl_log)
COVERAGE_MFILE := $(COVERAGE_SRCFILE:.tcl=.tcl_m)

# compatibility version-related files
COMPAT_DIR := compat

# source definitions shared across the Makefiles of this project
ifneq ($(wildcard Makefile.inc),Makefile.inc)
  $(error Makefile.inc is missing, please run './configure')
endif
include Makefile.inc

all: initdir pkgdoc modulecmd.tcl ChangeLog README \
	contrib/scripts/add.modules
ifeq ($(compatversion),y)
all: $(COMPAT_DIR)/modulecmd $(COMPAT_DIR)/ChangeLog
else
endif

initdir: version.inc
	make -C init all

pkgdoc: version.inc
	make -C doc man txt

doc: version.inc
	make -C doc all

www:
	make -C www all

# build version.inc shared definitions from git repository info
ifeq ($(wildcard .git) $(wildcard version.inc.in),.git version.inc.in)
GIT_CURRENT_TAG := $(shell git describe --tags --abbrev=0)
GIT_CURRENT_DESC := $(shell git describe --tags --dirty=-wip)
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
# retrieve all parts of the release number and increase minor release number
MODULES_RELEASE_BASE := $(firstword $(subst -, ,$(MODULES_RELEASE)))
ifeq ($(MODULES_RELEASE),$(MODULES_RELEASE_BASE))
MODULES_RELEASE_SUFFIX :=
else
MODULES_RELEASE_SUFFIX := $(subst $(MODULES_RELEASE_BASE)-,,$(MODULES_RELEASE))
endif
MODULES_LAST_RPM_VERSREL := $(shell grep -Pzo -m 1 '%changelog\n+\*.* - \K.*\n' \
	contrib/rpm/environment-modules.spec.in)

MODULES_LAST_RPM_RELEASE := $(subst $(MODULES_RELEASE_BASE)-,,$(MODULES_LAST_RPM_VERSREL))
ifneq ($(MODULES_RELEASE_SUFFIX),)
MODULES_LAST_RPM_RELEASE := $(subst .$(MODULES_RELEASE_SUFFIX),,$(MODULES_LAST_RPM_RELEASE))
endif

MODULES_LAST_RPM_RELEASE_P1 := $(firstword $(subst ., ,$(MODULES_LAST_RPM_RELEASE)))
MODULES_LAST_RPM_RELEASE_P2 := $(subst $(MODULES_LAST_RPM_RELEASE_P1).,,$(MODULES_LAST_RPM_RELEASE))

ifeq ($(MODULES_LAST_RPM_RELEASE_P2),)
MODULES_RPM_RELEASE_P2 := 1
else
MODULES_RPM_RELEASE_P2 := $(shell echo $$(($(MODULES_LAST_RPM_RELEASE_P2)+1)))
endif

MODULES_RPM_RELEASE := $(MODULES_LAST_RPM_RELEASE_P1).$(MODULES_RPM_RELEASE_P2)
ifneq ($(MODULES_RELEASE_SUFFIX),)
MODULES_RPM_RELEASE := $(MODULES_RPM_RELEASE).$(MODULES_RELEASE_SUFFIX)
endif
MODULES_RPM_RELEASE := $(MODULES_RPM_RELEASE)$(subst -,.,$(MODULES_BUILD))

else
# source version definitions shared across the Makefiles of this project
ifneq ($(wildcard version.inc),version.inc)
  $(error version.inc is missing)
endif
include version.inc
endif

define translate-in-script
sed -e 's|@prefix@|$(prefix)|g' \
	-e 's|@libexecdir@|$(libexecdir)|g' \
	-e 's|@TCLSHDIR@/tclsh|$(TCLSH)|g' \
	-e 's|@TCLSH@|$(TCLSH)|g' \
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

contrib/scripts/add.modules: contrib/scripts/add.modules.in
	$(translate-in-script)

# compatibility version-related rules
$(COMPAT_DIR)/modulecmd $(COMPAT_DIR)/ChangeLog:
	make -C $(COMPAT_DIR) $(@F)

ifeq ($(compatversion),y)
install: modulecmd.tcl ChangeLog README contrib/scripts/add.modules \
	$(COMPAT_DIR)/modulecmd $(COMPAT_DIR)/ChangeLog
else
install: modulecmd.tcl ChangeLog README contrib/scripts/add.modules
endif
	mkdir -p $(DESTDIR)$(libexecdir)
	mkdir -p $(DESTDIR)$(bindir)
	cp modulecmd.tcl $(DESTDIR)$(libexecdir)/
	chmod +x $(DESTDIR)$(libexecdir)/modulecmd.tcl
ifeq ($(compatversion),y)
	cp $(COMPAT_DIR)/modulecmd $(DESTDIR)$(libexecdir)/modulecmd-compat
	chmod +x $(DESTDIR)$(libexecdir)/modulecmd-compat
endif
	cp contrib/envml $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/envml
	cp contrib/scripts/add.modules $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/add.modules
	cp contrib/scripts/mkroot $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/mkroot
ifeq ($(docinstall),y)
	mkdir -p $(DESTDIR)$(docdir)
	cp COPYING.GPLv2 $(DESTDIR)$(docdir)/
	cp ChangeLog $(DESTDIR)$(docdir)/
	cp NEWS $(DESTDIR)$(docdir)/
	cp README $(DESTDIR)$(docdir)/
ifeq ($(compatversion),y)
	cp $(COMPAT_DIR)/ChangeLog $(DESTDIR)$(docdir)/ChangeLog-compat
	cp $(COMPAT_DIR)/NEWS $(DESTDIR)$(docdir)/NEWS-compat
endif
endif
	make -C init install DESTDIR=$(DESTDIR)
	make -C doc install DESTDIR=$(DESTDIR)
	@echo
	@echo "NOTICE: Modules installation is complete."
	@echo "        Please read the 'Configuration' section in INSTALL.txt to learn"
	@echo "        how to adapt your installation and make it fit your needs." >&2
	@echo

uninstall:
	rm -f $(DESTDIR)$(libexecdir)/modulecmd.tcl
ifeq ($(compatversion),y)
	rm -f $(DESTDIR)$(libexecdir)/modulecmd-compat
endif
	rm -f $(DESTDIR)$(bindir)/envml
	rm -f $(DESTDIR)$(bindir)/add.modules
	rm -f $(DESTDIR)$(bindir)/mkroot
ifeq ($(docinstall),y)
	rm -f $(addprefix $(DESTDIR)$(docdir)/,ChangeLog NEWS README COPYING.GPLv2)
ifeq ($(compatversion),y)
	rm -f $(addprefix $(DESTDIR)$(docdir)/,ChangeLog-compat NEWS-compat)
endif
endif
	make -C init uninstall DESTDIR=$(DESTDIR)
	make -C doc uninstall DESTDIR=$(DESTDIR)
	rmdir $(DESTDIR)$(libexecdir)
	rmdir $(DESTDIR)$(bindir)
	rmdir $(DESTDIR)$(datarootdir)
	rmdir --ignore-fail-on-non-empty $(DESTDIR)$(prefix)

# include pre-generated documents not to require documentation build
# tools when installing from dist tarball
dist-tar: ChangeLog README version.inc contrib/rpm/environment-modules.spec pkgdoc
	git archive --prefix=$(DIST_PREFIX)/ --worktree-attributes \
		-o $(DIST_PREFIX).tar HEAD
	cp doc/build/MIGRATING.txt  doc/build/INSTALL.txt ./
	tar -rf $(DIST_PREFIX).tar --transform 's,^,$(DIST_PREFIX)/,' \
		ChangeLog README MIGRATING.txt INSTALL.txt version.inc \
		doc/build/MIGRATING.txt doc/build/diff_v3_v4.txt \
		doc/build/INSTALL.txt \
		doc/build/module.1.in doc/build/modulefile.4 \
		contrib/rpm/environment-modules.spec
ifeq ($(compatversion) $(wildcard $(COMPAT_DIR)),y $(COMPAT_DIR))
	make -C $(COMPAT_DIR) distdir
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
	rm -f modulecmd.tcl
	rm -f contrib/scripts/add.modules
	rm -f modules-*.tar modules-*.tar.gz modules-*.tar.bz2
	rm -f modules-*.srpm
	make -C init clean
	make -C doc clean
ifeq ($(wildcard .git) $(wildcard version.inc.in),.git version.inc.in)
	rm -f version.inc
	rm -f contrib/rpm/environment-modules.spec
endif
ifneq ($(wildcard $(COMPAT_DIR)/Makefile),)
	make -C compat clean
endif
ifneq ($(wildcard www),)
	make -C www clean
endif

distclean: clean
	rm -f Makefile.inc
	rm -f site.exp
	rm -rf $(NAGELFAR_RELEASE)
ifeq ($(wildcard .git) $(wildcard $(COMPAT_DIR)),.git $(COMPAT_DIR))
	rm -rf $(COMPAT_DIR)
	git worktree prune
endif

ifeq ($(compatversion) $(wildcard $(COMPAT_DIR)),y $(COMPAT_DIR))
test: modulecmd.tcl $(COMPAT_DIR)/modulecmd
	make -C $(COMPAT_DIR) test
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


# install code coverage tool
$(NAGELFAR):
	wget http://downloads.sourceforge.net/nagelfar/$(NAGELFAR_RELEASE).tar.gz
	tar xzf $(NAGELFAR_RELEASE).tar.gz
	rm $(NAGELFAR_RELEASE).tar.gz

# instrument source file for code coverage
%.tcl_i: %.tcl $(NAGELFAR)
	$(NAGELFAR) -instrument $<

# prepare for code coverage run and make sure coverage log is clear
instrument: $(COVERAGE_IFILE)
	rm -f $(COVERAGE_LOGFILE)

# run tests on instrumented file to create coverage log
$(COVERAGE_LOGFILE): $(COVERAGE_IFILE)
	MODULECMD=./$< make test

# create markup file for better read coverage result
%.tcl_m: %.tcl_log $(NAGELFAR)
	$(NAGELFAR) -markup $*.tcl

testcoverage: instrument $(COVERAGE_MFILE)

testsyntax: $(NAGELFAR) $(COVERAGE_SRCFILE)
	$(NAGELFAR) -len 78 $(COVERAGE_SRCFILE)
