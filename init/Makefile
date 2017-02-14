SH_LIKE  = sh ksh bash zsh
CSH_LIKE = csh tcsh tcsh_completion
OTHER    = perl python lisp tcl fish

# get MODULESHOME location from this Makefile location
MODULESHOME := $(abspath $(lastword $(MAKEFILE_LIST))/../..)
# get tclsh location from standard PATHs or /usr/local/bin
TCLSH := $(shell PATH=/usr/bin:/bin:/usr/local/bin command -v tclsh)

ALL_SHELLS = $(SH_LIKE) $(CSH_LIKE) $(OTHER)

all: $(ALL_SHELLS) modulerc.fake

define translate-in-script
perl -pe 's|\@MODULESHOME\@|$(MODULESHOME)|g; \
	s|\@SHELLNAME\@|$@|g; \
	s|\@TCLSH\@|$(TCLSH)|g' $< > $@
endef

%: %.in Makefile
	$(translate-in-script)

# tcsh is derivated from csh init script
tcsh: csh.in Makefile
	$(translate-in-script)

clean:
	rm -f $(ALL_SHELLS)

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
