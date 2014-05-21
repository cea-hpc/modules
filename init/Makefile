SH_LIKE  = sh ksh bash zsh
CSH_LIKE = csh tcsh
OTHER    = perl python lisp tcl
D =  $(shell pwd)

ALL_SHELLS = $(SH_LIKE) $(CSH_LIKE) $(OTHER)

all: $(ALL_SHELLS) modulerc.fake

%: %.in Makefile
	perl -MCwd -MFile::Basename -pe 'BEGIN {$$d=dirname(cwd)} s|\$$MODULESHOME|$$d|g' $< > $@

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
