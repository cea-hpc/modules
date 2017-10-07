.PHONY: install uninstall clean

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

# source version definitions shared across the Makefiles of this project
include ../version.inc

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
  setmanpathre := s|@setmanpath@||g
else
  setmanpathre := /@setmanpath@/d
endif
ifeq ($(setbinpath),y)
  setbinpathre := s|@setbinpath@||g
else
  setbinpathre := /@setbinpath@/d
endif
ifeq ($(compatversion),y)
  compatversionre := s|@compatversion@||g
  notcompatversionre := /@notcompatversion@/d
else
  compatversionre := /@compatversion@/d
  notcompatversionre := s|@notcompatversion@||g
endif

define translate-in-script
sed -e 's|@prefix@|$(prefix)|g' \
	-e 's|@libexecdir@|$(libexecdir)|g' \
	-e 's|@initdir@|$(initdir)|g' \
	-e 's|@modulefilesdir@|$(modulefilesdir)|g' \
	-e 's|@bindir@|$(bindir)|g' \
	-e 's|@mandir@|$(mandir)|g' \
	-e '$(setmanpathre)' \
	-e '$(setbinpathre)' \
	-e '$(compatversionre)' \
	-e '$(notcompatversionre)' \
	-e 's|@modulerc@|$(modulerc)|g' \
	-e 's|@modulepath@|$(modulepath)|g' \
	-e 's|@SHELLNAME@|$@|g' \
	-e 's|@VERSION@|$(MODULES_RELEASE)$(MODULES_BUILD)|g' \
	-e 's|@TCLSH@|$(TCLSH)|g' $< > $@
endef

# avoid shared definitions to be rebuilt by make
../Makefile.inc ../version.inc: ;

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

$(EXAMPLE_MODFILES_SRCDIR)/modules: $(EXAMPLE_MODFILES_SRCDIR)/modules.in ../version.inc
	$(translate-in-script)

install: all
	mkdir -p $(DESTDIR)$(initdir)
	mkdir -p $(DESTDIR)$(initdir)/ksh-functions
	mkdir -p $(DESTDIR)$(modulefilesdir)
	cp $(ALL_SHELLS) $(DESTDIR)$(initdir)/
	cp fish_completion $(DESTDIR)$(initdir)/
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
	rm -f $(addprefix $(DESTDIR)$(initdir)/,$(ALL_SHELLS) fish_completion)
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
