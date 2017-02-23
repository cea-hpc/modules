.PHONY: install uninstall clean .makeinstallpath

SH_LIKE  = sh ksh bash zsh profile.sh
CSH_LIKE = csh tcsh tcsh_completion profile.csh
OTHER    = perl python lisp tcl fish

# load previously saved install paths if any
-include .makeinstallpath

# set default installation paths if not yet defined
prefix ?= /usr/local/modules-tcl
bindir ?= $(prefix)/bin
libexecdir ?= $(prefix)/libexec
initdir ?= $(prefix)/init
modulefilesdir ?=$(prefix)/modulefiles
datarootdir ?= $(prefix)/share
mandir ?= $(datarootdir)/man
# modulepaths and modulefiles to enable in default config
modulepath ?= $(modulefilesdir)
loadedmodules ?=

# enable or not some specific definition
setmanpath ?= y
setbinpath ?= y

# kind of config file to use: modulerc or both .modulespath and modulerc
setdotmodulespath ?= n
ifeq ($(setdotmodulespath),y)
  ALL_CONFIG := .modulespath modulerc
  # modulepath information will only be set in .modulespath
else
  ALL_CONFIG := modulerc
  # generate 'module use' commands to put in modulerc from modulepath
  modulerc := $(addprefix \nmodule use --append ,$(subst :, ,$(modulepath)))
endif

# generate 'module load' commands to put in modulerc from loadedmodules
ifneq ($(loadedmodules),)
  modulerc := $(modulerc)$(addprefix \nmodule load ,\
    $(subst :, ,$(loadedmodules)))
endif

# get tclsh location from standard PATHs or /usr/local/bin
TCLSH := $(shell PATH=/usr/bin:/bin:/usr/local/bin command -v tclsh)

ALL_SHELLS = $(SH_LIKE) $(CSH_LIKE) $(OTHER)

all: $(ALL_SHELLS) $(ALL_CONFIG) .makeinstallpath

# save defined install paths
.makeinstallpath:
	@echo "prefix := $(prefix)" >$@
	@echo "bindir := $(bindir)" >>$@
	@echo "libexecdir := $(libexecdir)" >>$@
	@echo "initdir := $(initdir)" >>$@
	@echo "modulefilesdir := $(modulefilesdir)" >>$@
	@echo "datarootdir := $(datarootdir)" >>$@
	@echo "mandir := $(mandir)" >>$@
	@echo "setdotmodulespath := $(setdotmodulespath)" >>$@

# if enabled translate to keep text after markup elsewhere remove the
# entire line
ifeq ($(setmanpath),y)
  setmanpathre := \1\n
else
  setmanpathre :=
endif
ifeq ($(setbinpath),y)
  setbinpathre := \1\n
else
  setbinpathre :=
endif

define translate-in-script
perl -pe 's|\@prefix\@|$(prefix)|g; \
	s|\@libexecdir\@|$(libexecdir)|g; \
	s|\@initdir\@|$(initdir)|g; \
	s|\@modulefilesdir\@|$(modulefilesdir)|g; \
	s|\@bindir\@|$(bindir)|g; \
	s|\@mandir\@|$(mandir)|g; \
	s|\@setmanpath\@(.*)\n|$(setmanpathre)|g; \
	s|\@setbinpath\@(.*)\n|$(setbinpathre)|g; \
	s|\@modulerc\@|$(modulerc)|g; \
	s|\@modulepath\@|$(modulepath)|g; \
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
	mkdir -p $(modulefilesdir)
	cp $(ALL_SHELLS) $(initdir)/
	cp bash_completion $(initdir)/
	cp ksh $(initdir)/ksh-functions/module
	cp $(ALL_CONFIG) $(initdir)/

uninstall:
	rm -f $(addprefix $(initdir)/,$(ALL_SHELLS) bash_completion ksh-functions/module)
	rm -f $(addprefix $(initdir)/,$(ALL_CONFIG))
	rmdir $(modulefilesdir)
	rmdir $(initdir)/ksh-functions
	rmdir $(initdir)

clean:
	rm -f $(ALL_SHELLS) modulerc .modulespath .makeinstallpath
