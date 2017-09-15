.PHONY: install uninstall clean

VERSION_FILE := ../modulecmd.tcl.in
CURRENT_VERSION := $(shell grep '^set MODULES_CURRENT_VERSION' \
	$(VERSION_FILE) | cut -d ' ' -f 3)

SH_LIKE  = sh ksh bash zsh profile.sh bash_completion
CSH_LIKE = csh tcsh tcsh_completion profile.csh
OTHER    = perl.pm python.py ruby.rb lisp tcl fish cmake r.R

# example modulefiles to install
EXAMPLE_MODFILES_SRCDIR := ../contrib/modulefiles
EXAMPLE_MODFILES = dot module-git module-info modules null use.own

# source definitions shared across the Makefiles of this project
ifneq ($(wildcard ../Makefile.inc),../Makefile.inc)
  $(error ../Makefile.inc is missing, please run '../configure')
endif
include ../Makefile.inc

# kind of config file to use: modulerc or .modulespath
ifeq ($(setdotmodulespath),y)
  # both configuration files will be installed and in this case default init
  # scripts will use .modulespath prior to modulerc. If .modulespath sets a
  # module path then modulerc will be ignored
  ALL_CONFIG := .modulespath modulerc
else
  ALL_CONFIG := modulerc
endif

# generate 'module use' commands to put in modulerc from modulepath
modulerc := $(addprefix \nmodule use --append ,$(subst :, ,$(modulepath)))
# generate 'module load' commands to put in modulerc from loadedmodules
ifneq ($(loadedmodules),)
  modulerc := $(modulerc)$(addprefix \nmodule load ,\
    $(subst :, ,$(loadedmodules)))
endif

ifeq ($(compatversion),y)
SH_LIKE += profile-compat.sh
CSH_LIKE += profile-compat.csh
endif

ALL_SHELLS = $(SH_LIKE) $(CSH_LIKE) $(OTHER)

all: $(ALL_SHELLS) $(ALL_CONFIG) $(EXAMPLE_MODFILES_SRCDIR)/modules

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
ifeq ($(compatversion),y)
  compatversionre := \1\n
  notcompatversionre :=
else
  compatversionre :=
  notcompatversionre := \1\n
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
	s|\@compatversion\@(.*)\n|$(compatversionre)|g; \
	s|\@notcompatversion\@(.*)\n|$(notcompatversionre)|g; \
	s|\@modulerc\@|$(modulerc)|g; \
	s|\@modulepath\@|$(modulepath)|g; \
	s|\@SHELLNAME\@|$@|g; \
	s|\@VERSION\@|$(CURRENT_VERSION)|g; \
	s|\@TCLSH\@|$(TCLSH)|g' $< > $@
endef

# avoid shared definitions to be rebuilt by make
../Makefile.inc: ;

%: %.in
	$(translate-in-script)

# tcsh is derivated from csh init script
tcsh: csh.in
	$(translate-in-script)

# this rule is needed for profile.sh to get matched
profile.sh: profile.sh.in
	$(translate-in-script)

# this rule is needed for profile-compat.sh to get matched
profile-compat.sh: profile-compat.sh.in
	$(translate-in-script)

$(EXAMPLE_MODFILES_SRCDIR)/modules: $(EXAMPLE_MODFILES_SRCDIR)/modules.in
	$(translate-in-script)

install: all
	mkdir -p $(DESTDIR)$(initdir)
	mkdir -p $(DESTDIR)$(initdir)/ksh-functions
	mkdir -p $(DESTDIR)$(modulefilesdir)
	cp $(ALL_SHELLS) $(DESTDIR)$(initdir)/
	cp bash_completion $(DESTDIR)$(initdir)/
	cp -r zsh-functions $(DESTDIR)$(initdir)/
	cp ksh $(DESTDIR)$(initdir)/ksh-functions/module
ifeq ($(compatversion),y)
	cp ksh $(DESTDIR)$(initdir)/ksh-functions/switchml
else
	rm -f $(DESTDIR)$(initdir)/ksh-functions/switchml
endif
	cp -P --remove-destination $(ALL_CONFIG) $(DESTDIR)$(initdir)/
ifeq ($(examplemodulefiles),y)
	cp $(addprefix $(EXAMPLE_MODFILES_SRCDIR)/,$(EXAMPLE_MODFILES)) $(DESTDIR)$(modulefilesdir)/
endif

uninstall:
	rm -f $(addprefix $(DESTDIR)$(initdir)/,$(ALL_SHELLS) bash_completion)
	rm -f $(DESTDIR)$(initdir)/ksh-functions/module
ifeq ($(compatversion),y)
	rm -f $(DESTDIR)$(initdir)/ksh-functions/switchml
endif
	rm -f $(DESTDIR)$(initdir)/zsh-functions/_module
	rm -f $(addprefix $(DESTDIR)$(initdir)/,$(ALL_CONFIG))
ifeq ($(wildcard $(DESTDIR)$(modulefilesdir)/*),)
	rmdir $(DESTDIR)$(modulefilesdir)
else
	@echo
	@echo "WARNING: $(DESTDIR)$(modulefilesdir) is not empty so skip removal" >&2
	@echo
endif
	rmdir $(DESTDIR)$(initdir)/ksh-functions
	rmdir $(DESTDIR)$(initdir)/zsh-functions
	rmdir $(DESTDIR)$(initdir)

clean:
	rm -f $(ALL_SHELLS) modulerc .modulespath $(EXAMPLE_MODFILES_SRCDIR)/modules
