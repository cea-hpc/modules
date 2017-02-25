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
# path to C version installation, unknown by default
cverinitdir ?=
cverbindir ?=
cvermandir ?=

# enable or not some specific definition
setmanpath ?= y
setbinpath ?= y

# define switchml if we know where C version is installed
# same applies for using C version .modulespath config
ifeq ($(cverinitdir),)
  override setswitchml := n
  override usecverdotmodulespath := n
else
  setswitchml ?= y
  usecverdotmodulespath ?= n
endif
# reset C version path if they are standard system path (to ensure
# switchml will not remove them from PATH or MANPATH)
ifneq ($(findstring $(cverbindir),/bin /usr/bin /usr/local/bin),)
  override cverbindir :=
endif
ifneq ($(findstring $(cvermandir),/usr/share/man /usr/local/share/man),)
  override cvermandir :=
endif

# kind of config file to use: modulerc or both .modulespath and modulerc
ifeq ($(usecverdotmodulespath),y)
  override setdotmodulespath := y
else
  setdotmodulespath ?= n
endif
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
	@echo "usecverdotmodulespath := $(usecverdotmodulespath)" >>$@
	@echo "cverinitdir := $(cverinitdir)" >>$@

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
ifeq ($(setswitchml),y)
  setswitchmlre := \1\n
else
  setswitchmlre :=
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
	s|\@setswitchml\@(.*)\n|$(setswitchmlre)|g; \
	s|\@cverinitdir\@|$(cverinitdir)|g; \
	s|\@cverbindir\@|$(cverbindir)|g; \
	s|\@cvermandir\@|$(cvermandir)|g; \
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

# make link on C version installed .modulespath
ifeq ($(usecverdotmodulespath),y)
.modulespath:
	ln -s $(cverinitdir)/$@ $@
endif

install: all
	mkdir -p $(initdir)
	mkdir -p $(initdir)/ksh-functions
	mkdir -p $(modulefilesdir)
	cp $(ALL_SHELLS) $(initdir)/
	cp bash_completion $(initdir)/
	cp ksh $(initdir)/ksh-functions/module
	cp ksh $(initdir)/ksh-functions/switchml
	cp -P --remove-destination $(ALL_CONFIG) $(initdir)/

uninstall:
	rm -f $(addprefix $(initdir)/,$(ALL_SHELLS) bash_completion)
	rm -f $(addprefix $(initdir)/,ksh-functions/module ksh-functions/switchml)
	rm -f $(addprefix $(initdir)/,$(ALL_CONFIG))
	rmdir $(modulefilesdir)
	rmdir $(initdir)/ksh-functions
	rmdir $(initdir)

clean:
	rm -f $(ALL_SHELLS) modulerc .modulespath .makeinstallpath
