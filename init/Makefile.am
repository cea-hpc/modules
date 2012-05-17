###########################################################################
##
## File:        init/Makefile.am
## Versions:    $Id: Makefile.am,v 1.2.4.2 2011/10/17 17:16:52 rkowen Exp $
## Created:     2002/06/14
##
###########################################################################


AUTOMAKE_OPTIONS=foreign no-installinfo

EXTRA_SCRIPTS=							\
	sh ksh zsh bash bash_completion				\
	csh tcsh						\
	perl.pm python.py ruby.rb cmake				\
	.modulespath

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
	   echo " $(INSTALL_PROGRAM_ENV) $(INSTALL_DATA) $$p $(DESTDIR)$(exec_prefix)/init/$$p"; \
	   $(INSTALL_PROGRAM_ENV) $(INSTALL_DATA) $$p $(DESTDIR)$(exec_prefix)/init/$$p; \
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

# We need to filter the *.in files to produce the init scripts
# since we don't know the value of '@prefix@' until Make time.
# (We can not rely on configure to give us correct values.)
% : %.in $(top_builddir)/config.status
	sed -e "/@$(if $(subst 0,,$(WANTS_VERSIONING)),NOT,)VERSIONING\@/d; \
	s,@$(if $(subst 0,,$(WANTS_VERSIONING)),,NOT)VERSIONING\@,,g; \
	s,@prefix\@,${prefix},g; \
	s,@bindir\@,${exec_prefix}/bin,g; \
	s,@VERSION\@,@VERSION@,g; \
	s,@BASEPREFIX\@,@BASEPREFIX@,g;" < $< > $@

tcsh : csh
	cp csh $@

check :

clean :
	rm -f ${INIT_FILES}
