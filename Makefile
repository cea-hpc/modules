.PHONY: doc pkgdoc www initdir install uninstall dist srpm clean distclean \
	test testinstall instrument testcoverage

CURRENT_VERSION := $(shell grep '^set MODULES_CURRENT_VERSION' \
	modulecmd.tcl.in | cut -d ' ' -f 3)
DIST_PREFIX := modules-tcl-$(CURRENT_VERSION)

# definitions for code coverage
NAGELFAR_RELEASE := nagelfar125
NAGELFAR := $(NAGELFAR_RELEASE)/nagelfar.tcl
COVERAGE_SRCFILE := modulecmd.tcl
COVERAGE_IFILE := $(COVERAGE_SRCFILE:.tcl=.tcl_i)
COVERAGE_LOGFILE := $(COVERAGE_SRCFILE:.tcl=.tcl_log)
COVERAGE_MFILE := $(COVERAGE_SRCFILE:.tcl=.tcl_m)

# source definitions shared across the Makefiles of this project
ifneq ($(wildcard Makefile.inc),Makefile.inc)
  $(error Makefile.inc is missing, please run './configure')
endif
include Makefile.inc

all: initdir pkgdoc modulecmd.tcl ChangeLog README

initdir:
	make -C init all

pkgdoc:
	make -C doc man txt

doc:
	make -C doc all

www:
	make -C www all

# avoid shared definitions to be rebuilt by make
Makefile.inc: ;

modulecmd.tcl: modulecmd.tcl.in
	perl -pe 's|\@TCLSHDIR\@/tclsh|$(TCLSH)|g;' $< > $@
	chmod +x $@

ChangeLog:
	contrib/gitlog2changelog.py

README:
	perl -pe 's|^\[\!\[.*\].*\n||;' $@.md > $@

install: modulecmd.tcl ChangeLog README
	mkdir -p $(DESTDIR)$(libexecdir)
	mkdir -p $(DESTDIR)$(bindir)
	cp modulecmd.tcl $(DESTDIR)$(libexecdir)/
	chmod +x $(DESTDIR)$(libexecdir)/modulecmd.tcl
	cp contrib/envml $(DESTDIR)$(bindir)/
	chmod +x $(DESTDIR)$(bindir)/envml
ifeq ($(docinstall),y)
	mkdir -p $(DESTDIR)$(docdir)
	cp COPYING.GPLv2 $(DESTDIR)$(docdir)/
	cp ChangeLog $(DESTDIR)$(docdir)/
	cp NEWS $(DESTDIR)$(docdir)/
	cp README $(DESTDIR)$(docdir)/
	cp INSTALL.txt $(DESTDIR)$(docdir)/INSTALL
endif
	make -C init install DESTDIR=$(DESTDIR)
	make -C doc install DESTDIR=$(DESTDIR)
	@echo
	@echo "NOTICE: Modules-Tcl installation is complete."
	@echo "        Please read the 'Configuration' section in INSTALL.txt to learn"
	@echo "        how to adapt your installation and make it fit your needs." >&2
	@echo

uninstall:
	rm -f $(DESTDIR)$(libexecdir)/modulecmd.tcl
	rm -f $(DESTDIR)$(bindir)/envml
ifeq ($(docinstall),y)
	rm -f $(addprefix $(DESTDIR)$(docdir)/,ChangeLog NEWS README INSTALL COPYING.GPLv2)
endif
	make -C init uninstall DESTDIR=$(DESTDIR)
	make -C doc uninstall DESTDIR=$(DESTDIR)
	rmdir $(DESTDIR)$(libexecdir)
	rmdir $(DESTDIR)$(bindir)
	rmdir $(DESTDIR)$(datarootdir)
	rmdir --ignore-fail-on-non-empty $(DESTDIR)$(prefix)

dist: ChangeLog README
	git archive --prefix=$(DIST_PREFIX)/ --worktree-attributes \
		-o $(DIST_PREFIX).tar HEAD
	tar -rf $(DIST_PREFIX).tar --transform 's,^,$(DIST_PREFIX)/,' $^
	gzip -f -9 $(DIST_PREFIX).tar

srpm: dist
	rpmbuild -ts $(DIST_PREFIX).tar.gz

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
	rm -f modulecmd.tcl
	rm -f modules-tcl-*.tar.gz
	rm -f modules-tcl-*.srpm
	make -C init clean
	make -C doc clean
ifneq ($(wildcard www),)
	make -C www clean
endif

distclean: clean
	rm -f Makefile.inc
	rm -f site.exp
	rm -rf $(NAGELFAR_RELEASE)

test: modulecmd.tcl
	TCLSH=$(TCLSH); export TCLSH; \
	MODULEVERSION=Tcl; export MODULEVERSION; \
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
