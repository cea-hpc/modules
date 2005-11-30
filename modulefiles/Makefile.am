###########################################################################
##
## File:        modulefiles/Makefile.am
## Versions:    $Id: Makefile.am,v 1.3 2005/11/30 18:53:44 rkowen Exp $
## Created:     2002/06/14
##
###########################################################################


AUTOMAKE_OPTIONS=foreign no-installinfo

BASEMODULES= \
	dot module-cvs module-info modules null use.own

EXTRA_SCRIPTS= ${BASEMODULES} version

all:	${EXTRA_SCRIPTS}

@VERSIONING@ install-data-local : install-basemodules install-version
@NOTVERSIONING@ install-data-local : install-basemodules

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

install-version : version
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(VERSIONPATH)
	@if test x$(WANTS_VERSIONING) = x1 ; then \
	  echo " $(INSTALL_DATA_ENV) $(INSTALL_DATA) version $(DESTDIR)$(VERSIONPATH)/$(VERSION)"; \
	  $(INSTALL_DATA_ENV) $(INSTALL_DATA) version $(DESTDIR)$(VERSIONPATH)/$(VERSION) ; \
	fi

@VERSIONING@ uninstall-local : uninstall-basemodules uninstall-version
@NOTVERSIONING@ uninstall-local : uninstall-basemodules

uninstall-basemodules :
	@$(NORMAL_UNINSTALL)
	@if test x$(WANTS_VERSIONING) = x1 ; then \
	  list='$(BASEMODULES)'; for p in $$list; do \
	    echo " rm -f $(DESTDIR)$(prefix)/modulefiles/$$p"; \
	    rm -f $(DESTDIR)$(prefix)/modulefiles/$$p; \
	  done; \
	else \
	  echo "*** BASE MODULES are not uninstalled from $(DESTDIR)$(prefix)/modulefiles ***"; \
	  echo "*** Do This Yourself If you really want to ***"; \
	fi

uninstall-version :
	@$(NORMAL_UNINSTALL)
	@if test x$(WANTS_VERSIONING) = x1 ; then \
	    echo " rm -f $(DESTDIR)$(VERSIONPATH)/$(VERSION)" ; \
	    rm -f $(DESTDIR)$(VERSIONPATH)/$(VERSION) ; \
	else \
	  echo "*** VERSION modulefile $(DESTDIR)$(VERSIONPATH)/$(VERSION) not uninstalled ***"; \
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
