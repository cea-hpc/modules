###########################################################################
##
## File:        init/Makefile.am
## Versions:    $Id: Makefile.am,v 1.5 2006/06/01 21:20:10 rkowen Exp $
## Created:     2002/06/14
##
###########################################################################


AUTOMAKE_OPTIONS=foreign no-installinfo

EXTRA_SCRIPTS= \
	bash csh ksh perl python sh tcsh zsh ksh .modulespath

INIT_FILES=${EXTRA_SCRIPTS}

MAKEDIRS= $(initdir)

# all:    $(INIT_FILES)
#
# only make the init files when an install is done.
# why? because we want to get the latest $DESTDIR & $bindir
#
all:
	rm -f $(INIT_FILES)

install-exec-local : install-inits

install-inits : $(INIT_FILES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(exec_prefix)/init
	@list='$(INIT_FILES)'; for p in $$list; do \
	  if test -f $$p \
	  ; then \
	   echo " $(INSTALL_PROGRAM_ENV) $(INSTALL_PROGRAM) $$p $(DESTDIR)$(exec_prefix)/init/$$p"; \
	   $(INSTALL_PROGRAM_ENV) $(INSTALL_PROGRAM) $$p $(DESTDIR)$(exec_prefix)/init/$$p; \
	  else :; fi; \
	done
	rm -f $(INIT_FILES)

uninstall-local : uninstall-inits

uninstall-inits :
	@$(NORMAL_UNINSTALL)
	@list='$(INIT_FILES)'; for p in $$list; do \
	  echo " rm -f $(DESTDIR)$(exec_prefix)/init/$$p"; \
	  rm -f $(DESTDIR)$(exec_prefix)/init/$$p; \
	done

#
# we need to filter the *.in files to produce the init scripts
#   since we don't know the value of @prefix@ until Make time
#   therefore, we can not rely on configure to give us correct values.
# Cobbled together filter to substitute the configure macros
#   which are expanded and passed on the filter commandline.
#

#.modulespath : .modulespath.in
#	${SHELL} filter "@prefix@" "@bindir@" "@BASEPREFIX@" \
#		"@VERSION@" "@VERSIONING@" "@NOTVERSIONING@" \
#		< .modulespath.in > .modulespath

#.modulespath : .modulespath.in $(top_builddir)/config.status
#	(cd $(top_builddir); config.status init/.modulespath)

bash : bash.in $(top_builddir)/config.status
	${SHELL} $(srcdir)/filter "@prefix@" "@bindir@" "@BASEPREFIX@" \
		"@VERSION@" "@VERSIONING@" "@NOTVERSIONING@" \
		< bash.in > bash

csh : csh.in $(top_builddir)/config.status
	${SHELL} $(srcdir)/filter "@prefix@" "@bindir@" "@BASEPREFIX@" \
		"@VERSION@" "@VERSIONING@" "@NOTVERSIONING@" \
		< csh.in > csh

ksh : ksh.in $(top_builddir)/config.status
	${SHELL} $(srcdir)/filter "@prefix@" "@bindir@" "@BASEPREFIX@" \
		"@VERSION@" "@VERSIONING@" "@NOTVERSIONING@" \
		< ksh.in > ksh

perl : perl.in $(top_builddir)/config.status
	${SHELL} $(srcdir)/filter "@prefix@" "@bindir@" "@BASEPREFIX@" \
		"@VERSION@" "@VERSIONING@" "@NOTVERSIONING@" \
		< perl.in > perl

python : python.in $(top_builddir)/config.status
	${SHELL} $(srcdir)/filter "@prefix@" "@bindir@" "@BASEPREFIX@" \
		"@VERSION@" "@VERSIONING@" "@NOTVERSIONING@" \
		< python.in > python

sh : sh.in $(top_builddir)/config.status
	${SHELL} $(srcdir)/filter "@prefix@" "@bindir@" "@BASEPREFIX@" \
		"@VERSION@" "@VERSIONING@" "@NOTVERSIONING@" \
		< sh.in > sh

zsh : zsh.in $(top_builddir)/config.status
	${SHELL} $(srcdir)/filter "@prefix@" "@bindir@" "@BASEPREFIX@" \
		"@VERSION@" "@VERSIONING@" "@NOTVERSIONING@" \
		< zsh.in > zsh

tcsh : csh
	cp csh $@

check :

clean :
	rm -f ${INIT_FILES}
