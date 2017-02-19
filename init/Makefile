.PHONY: install clean .makeinstallpath

SH_LIKE  = sh ksh bash zsh profile.sh
CSH_LIKE = csh tcsh tcsh_completion profile.csh
OTHER    = perl python lisp tcl fish

# load previously saved install paths if any
-include .makeinstallpath

# set default installation paths if not yet defined
prefix ?= /usr/local/modules-tcl
libexecdir ?= $(prefix)/libexec
initdir ?= $(prefix)/init

# get tclsh location from standard PATHs or /usr/local/bin
TCLSH := $(shell PATH=/usr/bin:/bin:/usr/local/bin command -v tclsh)

ALL_SHELLS = $(SH_LIKE) $(CSH_LIKE) $(OTHER)

all: $(ALL_SHELLS) modulerc.fake .makeinstallpath

# save defined install paths
.makeinstallpath:
	@echo "prefix := $(prefix)" >$@
	@echo "libexecdir := $(libexecdir)" >>$@
	@echo "initdir := $(initdir)" >>$@

define translate-in-script
perl -pe 's|\@prefix\@|$(prefix)|g; \
	s|\@libexecdir\@|$(libexecdir)|g; \
	s|\@initdir\@|$(initdir)|g; \
	s|\@SHELLNAME\@|$@|g; \
	s|\@TCLSH\@|$(TCLSH)|g' $< > $@
endef

%: %.in Makefile
	$(translate-in-script)

# tcsh is derivated from csh init script
tcsh: csh.in Makefile
	$(translate-in-script)

# this rule is needed for profile.sh to get matched
profile.sh: profile.sh.in Makefile
	$(translate-in-script)

install: all
	mkdir -p $(initdir)
	mkdir -p $(initdir)/ksh-functions
	cp $(ALL_SHELLS) $(initdir)/
	cp bash_completion $(initdir)/
	cp ksh $(initdir)/ksh-functions/module

clean:
	rm -f $(ALL_SHELLS) .makeinstallpath

modulerc.fake:
ifeq ($(wildcard modulerc),) 
	@echo "\n\n\tinit/modulerc does not exist.  You should create one"
	@echo "\tsee init/modulerc.examp for an example file."
else
	@echo "\n\n\tyou should review your: init/modulerc"
endif
#	echo module use $(MODULESHOME)/modulefiles > modulerc
#	echo "module use ~/.modules" >> modulerc
#
