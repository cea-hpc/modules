###########################################################################
##
## File:        ext/Makefile.am
## Versions:    $Id: Makefile.am,v 1.3 2005/12/05 16:38:08 rkowen Exp $
## Created:     2002/06/14
##
###########################################################################


AUTOMAKE_OPTIONS=foreign dejagnu no-installinfo

EXTRA_scripts = add.ext mkroot

install-data-local : $(EXTRA_scripts)
	$(mkinstalldirs) $(DESTDIR)$(prefix)/bin
	@list='$(EXTRA_scripts)'; for p in $$list; do \
	  if test -f $$p \
	  ; then \
	   echo " $(INSTALL_PROGRAM_ENV) $(INSTALL_PROGRAM) $$p $(DESTDIR)$(prefix)/bin/$$p"; \
	   $(INSTALL_PROGRAM_ENV) $(INSTALL_PROGRAM) $$p $(DESTDIR)$(prefix)/bin/$$p; \
	  else :; fi; \
	done

uninstall-local :
	@list='$(EXTRA_scripts)'; for p in $$list; do \
	  echo " rm -f $(DESTDIR)$(prefix)/bin/$$p"; \
	  rm -f $(DESTDIR)$(prefix)/bin/$$p; \
	done

check :

clean :
