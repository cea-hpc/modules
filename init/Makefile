.PHONY: dist-win install install-testconfig uninstall-testconfig uninstall \
	clean

SH_LIKE  = sh ksh bash zsh profile.sh bash_completion
CSH_LIKE = csh tcsh tcsh_completion profile.csh
OTHER    = perl.pm python.py ruby.rb lisp tcl fish cmake r.R

# example modulefiles to install
EXAMPLE_MODFILES_SRCDIR := ../contrib/modulefiles
EXAMPLE_MODFILES = dot module-git module-info modules null use.own
EXAMPLE_WIN_MODFILES = module-git module-info null
EXAMPLE_TESTCONFIG_MODFILES = null

# source definitions shared across the Makefiles of this project
ifneq ($(wildcard ../Makefile.inc),../Makefile.inc)
  $(error ../Makefile.inc is missing, please run '../configure')
endif
include ../Makefile.inc

# generate 'module use' commands to put in modulerc from modulepath
modulerc := $(addprefix \\\nmodule use --append {,$(subst :,} ,$(modulepath)}))
# generate 'module load' commands to put in modulerc from loadedmodules
ifneq ($(loadedmodules),)
  modulerc := $(modulerc)$(addprefix \\\nmodule load ,\
    $(subst :, ,$(loadedmodules)))
endif

# setup summary echo rules unless silent mode set
ECHO_DIR_PREFIX = init/
ifneq ($(findstring s,$(MAKEFLAGS)),s)
ECHO_GEN = @echo ' ' GEN $(ECHO_DIR_PREFIX)$@;
endif

ifeq ($(compatversion),y)
SH_LIKE += profile-compat.sh
CSH_LIKE += profile-compat.csh
endif

ALL_SHELLS = $(SH_LIKE) $(CSH_LIKE) $(OTHER)

all: $(ALL_SHELLS) modulerc zsh-functions/_module $(EXAMPLE_MODFILES_SRCDIR)/modules
ifeq ($(versioning),y)
all: $(EXAMPLE_MODFILES_SRCDIR)/version
endif
ifeq ($(setdotmodulespath),y)
  # both configuration files will be installed and in this case default init
  # scripts will use .modulespath prior to modulerc. If .modulespath sets a
  # module path then modulerc will be ignored
all: .modulespath
endif

../version.inc:
	$(MAKE) --no-print-directory -C .. version.inc

# source version definitions shared across the Makefiles of this project
include ../version.inc

# define init configs location
ifeq ($(initconfin),etcdir)
  modulespath := $(etcdir)/modulespath
  initrc := $(etcdir)/initrc
else
  modulespath := $(initdir)/.modulespath
  initrc := $(initdir)/modulerc
endif

# if enabled translate to keep text after markup elsewhere remove the
# entire line
ifeq ($(setmanpath),y)
  setmanpathre := s|@setmanpath@||g
else
  setmanpathre := /@setmanpath@/d
endif
ifeq ($(appendmanpath),y)
  appendmanpathre := s|@appendmanpath@||g
  prependmanpathre := /@prependmanpath@/d
else
  appendmanpathre := /@appendmanpath@/d
  prependmanpathre := s|@prependmanpath@||g
endif
ifeq ($(setbinpath),y)
  setbinpathre := s|@setbinpath@||g
else
  setbinpathre := /@setbinpath@/d
endif
ifeq ($(appendbinpath),y)
  appendbinpathre := s|@appendbinpath@||g
  prependbinpathre := /@prependbinpath@/d
else
  appendbinpathre := /@appendbinpath@/d
  prependbinpathre := s|@prependbinpath@||g
endif
ifeq ($(compatversion),y)
  compatversionre := s|@compatversion@||g
  notcompatversionre := /@notcompatversion@/d
else
  compatversionre := /@compatversion@/d
  notcompatversionre := s|@notcompatversion@||g
endif
ifeq ($(usemanpath),y)
  usemanpathre := s|@usemanpath@||g
  notusemanpathre := /@notusemanpath@/d
else
  usemanpathre := /@usemanpath@/d
  notusemanpathre := s|@notusemanpath@||g
endif
ifeq ($(silentshdbgsupport),y)
  silentshdbgsupportre := s|@silentshdbgsupport@||g
else
  silentshdbgsupportre := /@silentshdbgsupport@/d
endif
ifeq ($(quarantinesupport),y)
  quarantinesupportre := s|@quarantinesupport@||g
  notquarantinesupportre := /@notquarantinesupport@/d
else
  quarantinesupportre := /@quarantinesupport@/d
  notquarantinesupportre := s|@notquarantinesupport@||g
endif

# comment entries if feature not enabled
ifeq ($(versioning),y)
  versdir := $(baseprefix)/versions
  setversioning :=
else
  setversioning := \#
endif

# define translation rules for quarantine mechanism
ifeq ($(quarantinevars),)
setquarvarsre := /@setquarvars@/d
notsetquarvarsre := s|@notsetquarvars@||g
else
setquarvarsre := s|@setquarvars@||g
notsetquarvarsre := /@notsetquarvars@/d
define runenvvarre
$(if $(filter-out ${1},${2}),s|\(@\(.*\)RUNENV_VAR\(.*\)RUNENV_VAL\(.*\)@\)|\2MODULES_RUNENV_${1}\3${2}\4\n\1|g;)
endef
quarvarsre = -e 's|@RUN_QUARANTINE@|$(foreach vv,$(quarantinevars),$(firstword\
	$(subst =, ,$(vv))))|g;
quarvarsre += $(foreach vv,$(quarantinevars),$(call runenvvarre,$(firstword\
	$(subst =, ,$(vv))),$(subst $(firstword $(subst =, ,$(vv)))=,,$(vv))))
quarvarsre += s/@.*RUNENV_VAR.*@//;'
endif

# define variables for shell completion
comp_cmds := add apropos aliases avail append-path clear config del display help initadd initclear initlist initprepend initrm is-loaded is-saved is-used is-avail info-loaded keyword list load path paths purge prepend-path refresh reload restore rm remove remove-path save savelist saveshow saverm search show sh-to-mod source swap switch test unload unuse use whatis
comp_opts := -D -h -s -T -v -V --debug --help --silent --trace --verbose --version --paginate --no-pager --color --color=
comp_load_opts := --auto --no-auto --force -f --icase -i
comp_list_opts := -a -j -l -o -t --all --json --long --output --output= --terse
comp_clear_opts := --force -f
comp_avail_opts := -a -C -d -i -j -L -l -o -S -t --all --contains --default --icase --json --latest --long --output --output= --starts-with --terse --indepth --no-indepth
comp_mfile_opts := -i --icase
comp_whatis_opts := -a -i -j --all --icase --json
comp_search_opts := -a -j --all --json
comp_aliases_opts := -a --all
comp_isavail_opts := -a -i --all --icase
comp_path_opts := -d --delim --duplicates
comp_rm_path_opts := -d --delim --index
comp_config_opts := --dump-state --reset advanced_version_spec auto_handling avail_indepth avail_output avail_terse_output collection_pin_version collection_target color colors contact extended_default extra_siteconfig home icase implicit_default implicit_requirement list_output list_terse_output locked_configs mcookie_version_check ml nearly_forbidden_days pager rcfile run_quarantine search_match set_shell_startup shells_with_ksh_fpath silent_shell_debug tag_abbrev tag_color_name term_background unload_match_order verbosity wa_277

define translate-in-script
$(ECHO_GEN)
sed -e 's|@prefix@|$(prefix)|g' \
	-e 's|@baseprefix@|$(baseprefix)|g' \
	-e 's|@libexecdir@|$(libexecdir)|g' \
	-e 's|@initdir@|$(initdir)|g' \
	-e 's|@etcdir@|$(etcdir)|g' \
	-e 's|@modulefilesdir@|$(modulefilesdir)|g' \
	-e 's|@bindir@|$(bindir)|g' \
	-e 's|@mandir@|$(mandir)|g' \
	-e 's|@comp_cmds@|$(comp_cmds)|g' \
	-e 's|@comp_opts@|$(comp_opts)|g' \
	-e 's|@comp_load_opts@|$(comp_load_opts)|g' \
	-e 's|@comp_list_opts@|$(comp_list_opts)|g' \
	-e 's|@comp_clear_opts@|$(comp_clear_opts)|g' \
	-e 's|@comp_avail_opts@|$(comp_avail_opts)|g' \
	-e 's|@comp_mfile_opts@|$(comp_mfile_opts)|g' \
	-e 's|@comp_whatis_opts@|$(comp_whatis_opts)|g' \
	-e 's|@comp_search_opts@|$(comp_search_opts)|g' \
	-e 's|@comp_aliases_opts@|$(comp_aliases_opts)|g' \
	-e 's|@comp_isavail_opts@|$(comp_isavail_opts)|g' \
	-e 's|@comp_path_opts@|$(comp_path_opts)|g' \
	-e 's|@comp_rm_path_opts@|$(comp_rm_path_opts)|g' \
	-e 's|@comp_config_opts@|$(comp_config_opts)|g' \
	-e '$(setmanpathre)' \
	-e '$(appendmanpathre)' \
	-e '$(prependmanpathre)' \
	-e '$(setbinpathre)' \
	-e '$(appendbinpathre)' \
	-e '$(prependbinpathre)' \
	-e '$(compatversionre)' \
	-e '$(notcompatversionre)' \
	-e '$(silentshdbgsupportre)' \
	-e '$(quarantinesupportre)' \
	-e '$(notquarantinesupportre)' \
	-e '$(usemanpathre)' \
	-e '$(notusemanpathre)' \
	-e $$'s|@modulerc@|$(modulerc)|g' \
	-e 's|@modulepath@|$(modulepath)|g' \
	-e '$(setquarvarsre)' $(quarvarsre) \
	-e '$(notsetquarvarsre)' \
	-e 's|@SHELLNAME@|$@|g' \
	-e 's|@VERSIONING@|$(setversioning)|g' \
	-e 's|@VERSION@|$(VERSION)|g' \
	-e 's|@SED_ERE@|$(SED_ERE)|g' \
	-e 's|@PS@|$(PS)|g' \
	-e 's|@BASENAME@|$(BASENAME)|g' \
	-e 's|@TCLSH@|$(TCLSH)|g' $< > $@
endef

# avoid shared definitions to be rebuilt by make
../Makefile.inc: ;

%: %.in
	$(translate-in-script)

# workaround '\\\n' issue on Linux with Make 3.81
modulerc: modulerc.in
	$(translate-in-script)
	mv $@ ${@}.tmp
	sed -e 's|\\$$||' ${@}.tmp > $@
	rm -f ${@}.tmp

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

$(EXAMPLE_MODFILES_SRCDIR)/version: $(EXAMPLE_MODFILES_SRCDIR)/version.in ../version.inc
	$(translate-in-script)

dist-win:
	mkdir $(DIST_WIN_PREFIX)/init
	cp cmd.cmd $(DIST_WIN_PREFIX)/init/
	mkdir $(DIST_WIN_PREFIX)/modulefiles
	cp $(addprefix $(EXAMPLE_MODFILES_SRCDIR)/,$(EXAMPLE_WIN_MODFILES)) $(DIST_WIN_PREFIX)/modulefiles/

# example configs for test rules
install-testconfig:
	mkdir -p $(DESTDIR)$(initdir)
	mkdir -p $(DESTDIR)$(etcdir)
	mkdir -p $(DESTDIR)$(prefix)/etc
	mkdir -p $(DESTDIR)$(modulefilesdir)
	mkdir -p $(DESTDIR)$(prefix)/test/modulefiles $(DESTDIR)$(prefix)/test/etc
	cp $(addprefix $(EXAMPLE_MODFILES_SRCDIR)/,$(EXAMPLE_TESTCONFIG_MODFILES)) $(DESTDIR)$(modulefilesdir)/

uninstall-testconfig:
	rmdir $(DESTDIR)$(prefix)/test/modulefiles $(DESTDIR)$(prefix)/test/etc $(DESTDIR)$(prefix)/test
	rm -f $(addprefix $(DESTDIR)$(modulefilesdir)/,$(EXAMPLE_TESTCONFIG_MODFILES))
	$(RMDIR_IGN_NON_EMPTY) $(DESTDIR)$(modulefilesdir) || true
	$(RMDIR_IGN_NON_EMPTY) $(DESTDIR)$(etcdir) || true
	$(RMDIR_IGN_NON_EMPTY) $(DESTDIR)$(initdir) || true

install: all
	mkdir -p $(DESTDIR)$(initdir)
	mkdir -p $(DESTDIR)$(initdir)/ksh-functions
	mkdir -p $(DESTDIR)$(initdir)/zsh-functions
	mkdir -p $(DESTDIR)$(modulefilesdir)
	cp $(ALL_SHELLS) $(DESTDIR)$(initdir)/
ifeq ($(windowssupport),y)
	cp cmd.cmd $(DESTDIR)$(initdir)/
endif
	cp fish_completion $(DESTDIR)$(initdir)/
	cp zsh-functions/_module $(DESTDIR)$(initdir)/zsh-functions/
	cp ksh $(DESTDIR)$(initdir)/ksh-functions/module
	cp ksh $(DESTDIR)$(initdir)/ksh-functions/ml
ifeq ($(compatversion),y)
	cp ksh $(DESTDIR)$(initdir)/ksh-functions/switchml
else
	rm -f $(DESTDIR)$(initdir)/ksh-functions/switchml
endif
	rm -f $(DESTDIR)$(modulespath) $(DESTDIR)$(initrc)
	cp -R -P modulerc $(DESTDIR)$(initrc)
ifeq ($(setdotmodulespath),y)
	cp -R -P .modulespath $(DESTDIR)$(modulespath)
endif
ifeq ($(examplemodulefiles),y)
	cp $(addprefix $(EXAMPLE_MODFILES_SRCDIR)/,$(EXAMPLE_MODFILES)) $(DESTDIR)$(modulefilesdir)/
endif
ifeq ($(versioning),y)
	mkdir -p $(DESTDIR)$(versdir)
	cp $(EXAMPLE_MODFILES_SRCDIR)/version $(DESTDIR)$(versdir)/$(VERSION)
endif

uninstall:
	rm -f $(addprefix $(DESTDIR)$(initdir)/,$(ALL_SHELLS) fish_completion)
ifeq ($(windowssupport),y)
	rm -f $(DESTDIR)$(initdir)/cmd.cmd
endif
	rm -f $(DESTDIR)$(initdir)/ksh-functions/module
	rm -f $(DESTDIR)$(initdir)/ksh-functions/ml
ifeq ($(compatversion),y)
	rm -f $(DESTDIR)$(initdir)/ksh-functions/switchml
endif
	rm -f $(DESTDIR)$(initdir)/zsh-functions/_module
	rm -f $(DESTDIR)$(modulespath) $(DESTDIR)$(initrc)
	$(RMDIR_IGN_NON_EMPTY) $(DESTDIR)$(modulefilesdir) || true
	@if [ -d $(DESTDIR)$(modulefilesdir) ]; then echo; echo "WARNING: $(DESTDIR)$(modulefilesdir) is not empty so skip removal" >&2; echo; fi
	rmdir $(DESTDIR)$(initdir)/ksh-functions
	rmdir $(DESTDIR)$(initdir)/zsh-functions
	$(RMDIR_IGN_NON_EMPTY) $(DESTDIR)$(initdir) || true
	@if [ -d $(DESTDIR)$(initdir) ]; then echo; echo "WARNING: $(DESTDIR)$(initdir) is not empty so skip removal" >&2; echo; fi
ifeq ($(versioning),y)
	rm -f $(DESTDIR)$(versdir)/$(VERSION)
	$(RMDIR_IGN_NON_EMPTY) $(DESTDIR)$(versdir) || true
	@if [ -d $(DESTDIR)$(versdir) ]; then echo; echo "WARNING: $(DESTDIR)$(versdir) is not empty so skip removal" >&2; echo; fi
endif

clean:
	rm -f $(ALL_SHELLS) modulerc zsh-functions/_module .modulespath $(EXAMPLE_MODFILES_SRCDIR)/modules
ifeq ($(versioning),y)
	rm -f $(EXAMPLE_MODFILES_SRCDIR)/version
endif

# quiet all commands unless verbose mode set
ifeq ($(VERBOSE),1)
V = 1
endif
# let verbose by default the install/clean/test and other specific non-build targets
$(V).SILENT: ../version.inc sh ksh bash zsh csh tcsh fish perl.pm python.py ruby.rb \
	lisp tcl cmake r.R bash_completion tcsh_completion zsh-functions/_module \
	profile.sh profile.csh profile-compat.sh profile-compat.csh .modulespath \
	modulerc $(EXAMPLE_MODFILES_SRCDIR)/modules $(EXAMPLE_MODFILES_SRCDIR)/version
