###########################################################################
##
## File:        modulefiles/Makefile.am
## Versions:    $Id: Makefile.am,v 1.1 2002/06/14 21:57:03 rkowen Exp $
## Created:     2002/06/14
##
###########################################################################


AUTOMAKE_OPTIONS=foreign no-installinfo

EXTRA_SCRIPTS= \
	dot module-cvs module-info modules null use.own version

BASEMODULES=${EXTRA_SCRIPTS}

all:	${BASEMODULES}

install-data-local : install-basemodules

install-basemodules : $(BASEMODULES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(prefix)/modulefiles
	@list='$(BASEMODULES)'; for p in $$list; do \
	  if test -f $$p \
	  ; then \
	   echo " $(INSTALL_DATA_ENV) $(INSTALL_DATA) $$p $(DESTDIR)$(prefix)/modulefiles/$$p"; \
	   $(INSTALL_DATA_ENV) $(INSTALL_DATA) $$p $(DESTDIR)$(prefix)/modulefiles/$$p; \
	  else :; fi; \
	done

uninstall-local : uninstall-basemodules

uninstall-basemodules :
	@$(NORMAL_UNINSTALL)
	@if test @WANTS_VERSIONING@ -eq 1 ; then \
	  list='$(BASEMODULES)'; for p in $$list; do \
	    echo " rm -f $(DESTDIR)$(prefix)/modulefiles/$$p"; \
	    rm -f $(DESTDIR)$(prefix)/modulefiles/$$p; \
	  done; \
	else \
	  echo "*** BASE MODULES are not uninstalled from $(DESTDIR)$(prefix)/modulefiles ***"; \
	  echo "*** Do This Yourself If you really want to ***"; \
	fi

##dot : dot.in ../config.status
##	(cd ..; config.status modulefiles/dot)
##
##module-cvs : module-cvs.in ../config.status
##	(cd ..; config.status modulefiles/module-cvs)
##
##module-info : module-info.in ../config.status
##	(cd ..; config.status modulefiles/module-info)
##
##modules : modules.in ../config.status
##	(cd ..; config.status modulefiles/modules)
##
##null : null.in ../config.status
##	(cd ..; config.status modulefiles/null)
##
##use.own : use.own.in ../config.status
##	(cd ..; config.status modulefiles/use.own)
##
##version : version.in ../config.status
##	(cd ..; config.status modulefiles/version)

check :

clean :
	rm -f ${BASEMODULES}
