.PHONY: install uninstall clean

.NOTPARALLEL:

SOURCE_DIR := source
BUILD_DIR := build

SPHINXOPTS := -q

# commands to install files
INSTALL = install
INSTALL_DIR = $(INSTALL) -d -m 755
INSTALL_DATA = $(INSTALL) -m 644

# source definitions shared across the Makefiles of this project
ifneq ($(wildcard ../Makefile.inc),../Makefile.inc)
  $(error ../Makefile.inc is missing, please run '../configure')
endif
include ../Makefile.inc

# quit if no sphinx-build found at configure time
ifeq ($(builddoc),n)
  $(error No 'sphinx-build' command defined, cannot build documentation)
endif

# setup summary echo rules unless silent mode set
ECHO_DIR_PREFIX = doc/
ifneq ($(findstring s,$(MAKEFLAGS)),s)
ECHO_GEN = @echo ' ' GEN $(ECHO_DIR_PREFIX)$@;
ECHO_GEN2 = @echo ' ' GEN
else
# disable echo rules followed by a string
ECHO_GEN2 = \#
endif

ALL_RST := $(wildcard $(SOURCE_DIR)/*.rst $(SOURCE_DIR)/cookbook/*.rst\
	$(SOURCE_DIR)/design/*.rst)
ALL_TXT := $(patsubst $(SOURCE_DIR)/%,$(BUILD_DIR)/%,$(patsubst %.rst,%.txt,$(ALL_RST)))
ALL_HTML := $(patsubst $(SOURCE_DIR)/%,$(BUILD_DIR)/%,$(patsubst %.rst,%.html,$(ALL_RST)))
ALL_MAN := $(BUILD_DIR)/module.1 $(BUILD_DIR)/ml.1 $(BUILD_DIR)/modulefile.5

all: man txt

# enable HTML pages build only from git repository
ifeq ($(wildcard ../.git),../.git)
ifneq ($(builddoc),p)
all: html
endif
endif

man: $(ALL_MAN)

ifneq ($(builddoc),p)
txt: $(ALL_TXT)
html: $(ALL_HTML)
else
# limit txt targets to documents that have been prebuilt
txt: $(BUILD_DIR)/changes.txt $(BUILD_DIR)/MIGRATING.txt \
	$(BUILD_DIR)/INSTALL.txt $(BUILD_DIR)/INSTALL-win.txt \
	$(BUILD_DIR)/NEWS.txt $(BUILD_DIR)/CONTRIBUTING.txt
endif

../version.inc:
	$(MAKE) --no-print-directory -C .. version.inc

# source version definitions shared across the Makefiles of this project
ifeq ($(findstring clean,$(MAKECMDGOALS)),)
-include ../version.inc
endif

define translate-in-script
$(ECHO_GEN)
sed -e 's|@prefix@|$(prefix)|g' \
	-e 's|@libexecdir@|$(libexecdir)|g' \
	-e 's|@etcdir@|$(etcdir)|g' \
	-e 's|@initdir@|$(initdir)|g' \
	-e 's|@modulefilesdir@|$(modulefilesdir)|g' \
	-e 's|@MODULES_RELEASE@|$(MODULES_RELEASE)|g' \
	-e 's|@MODULES_BUILD@|$(MODULES_BUILD)|g' \
	-e 's|@MODULES_BUILD_DATE@|$(MODULES_BUILD_DATE)|g' $< > $@
endef

# avoid shared definitions to be rebuilt by make
../Makefile.inc: ;

# rules to build docs from source are defined if docs are not found pre-built
# which happens when working from git repository or from a `git archive` dist
ifneq ($(builddoc),p)
$(SOURCE_DIR)/version.py: $(SOURCE_DIR)/version.py.in ../version.inc
	$(translate-in-script)

$(BUILD_DIR)/%.html: $(SOURCE_DIR)/%.rst $(SOURCE_DIR)/version.py
	$(ECHO_GEN2) $(ECHO_DIR_PREFIX)$(BUILD_DIR)/*.html
	$(SPHINXBUILD) $(SPHINXOPTS) -b html "$(SOURCE_DIR)" "$(BUILD_DIR)"

$(BUILD_DIR)/%.txt: $(SOURCE_DIR)/%.rst $(SOURCE_DIR)/version.py
	$(ECHO_GEN2) $(ECHO_DIR_PREFIX)$(BUILD_DIR)/*.txt
	$(SPHINXBUILD) $(SPHINXOPTS) -b text "$(SOURCE_DIR)" "$(BUILD_DIR)"

$(BUILD_DIR)/module.1.in $(BUILD_DIR)/modulefile.5 $(BUILD_DIR)/ml.1: $(ALL_RST) $(SOURCE_DIR)/version.py
	$(ECHO_GEN2) $(ECHO_DIR_PREFIX)$(BUILD_DIR)/ml.1
	$(ECHO_GEN2) $(ECHO_DIR_PREFIX)$(BUILD_DIR)/module.1.in
	$(ECHO_GEN2) $(ECHO_DIR_PREFIX)$(BUILD_DIR)/modulefile.5
	$(SPHINXBUILD) $(SPHINXOPTS) -t pathsubs -b man "$(SOURCE_DIR)" "$(BUILD_DIR)"
	$(if $(findstring module.1.in,$@),mv $(BUILD_DIR)/module.1 $@)
endif

$(BUILD_DIR)/module.1: $(BUILD_DIR)/module.1.in
	$(translate-in-script)

install: man txt
	$(INSTALL_DIR) '$(DESTDIR)$(mandir)/man1' '$(DESTDIR)$(mandir)/man5'
ifeq ($(docinstall),y)
	$(INSTALL_DIR) '$(DESTDIR)$(docdir)'
	$(INSTALL_DATA) $(BUILD_DIR)/changes.txt '$(DESTDIR)$(docdir)/'
	$(INSTALL_DATA) $(BUILD_DIR)/MIGRATING.txt '$(DESTDIR)$(docdir)/'
	$(INSTALL_DATA) $(BUILD_DIR)/INSTALL.txt '$(DESTDIR)$(docdir)/'
	$(INSTALL_DATA) $(BUILD_DIR)/INSTALL-win.txt '$(DESTDIR)$(docdir)/'
	$(INSTALL_DATA) $(BUILD_DIR)/NEWS.txt '$(DESTDIR)$(docdir)/'
	$(INSTALL_DATA) $(BUILD_DIR)/CONTRIBUTING.txt '$(DESTDIR)$(docdir)/'
endif
	$(INSTALL_DATA) $(BUILD_DIR)/module.1 '$(DESTDIR)$(mandir)/man1/'
	$(INSTALL_DATA) $(BUILD_DIR)/ml.1 '$(DESTDIR)$(mandir)/man1/'
	$(INSTALL_DATA) $(BUILD_DIR)/modulefile.5 '$(DESTDIR)$(mandir)/man5/'

uninstall:
ifeq ($(docinstall),y)
	rm -f '$(DESTDIR)$(docdir)/changes.txt'
	rm -f '$(DESTDIR)$(docdir)/MIGRATING.txt'
	rm -f '$(DESTDIR)$(docdir)/INSTALL.txt'
	rm -f '$(DESTDIR)$(docdir)/INSTALL-win.txt'
	rm -f '$(DESTDIR)$(docdir)/NEWS.txt'
	rm -f '$(DESTDIR)$(docdir)/CONTRIBUTING.txt'
	rmdir '$(DESTDIR)$(docdir)'
endif
	rm -f '$(DESTDIR)$(mandir)/man1/module.1'
	rm -f '$(DESTDIR)$(mandir)/man1/ml.1'
	rm -f '$(DESTDIR)$(mandir)/man5/modulefile.5'
	rmdir '$(DESTDIR)$(mandir)/man1' '$(DESTDIR)$(mandir)/man5'
	rmdir '$(DESTDIR)$(mandir)'

clean:
# do not clean generated docs if not in git repository
ifeq ($(wildcard ../.git),../.git)
ifeq ($(wildcard $(BUILD_DIR)),$(BUILD_DIR))
	rm -rf $(BUILD_DIR)
endif
	rm -f $(SOURCE_DIR)/version.py
endif
	rm -f $(BUILD_DIR)/module.1

# quiet all commands unless verbose mode set
ifeq ($(VERBOSE),1)
V = 1
endif
# let verbose by default the install/clean/test and other specific non-build targets
$(V).SILENT: ../version.inc $(SOURCE_DIR)/version.py $(ALL_HTML) $(ALL_TXT) \
	$(BUILD_DIR)/module.1.in $(ALL_MAN)
