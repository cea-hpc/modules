.PHONY: install uninstall clean

SOURCE_DIR := source
BUILD_DIR := build

SPHINXOPTS := -q

# compatibility version-related files
COMPAT_MAN_DIR := ../compat/doc

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
ALL_MAN := $(BUILD_DIR)/module.1 $(BUILD_DIR)/ml.1 $(BUILD_DIR)/modulefile.4

man: $(ALL_MAN)
ifeq ($(compatversion),y)
man: $(COMPAT_MAN_DIR)/module.1 $(COMPAT_MAN_DIR)/modulefile.4
endif

ifneq ($(builddoc),p)
txt: $(ALL_TXT)
html: $(ALL_HTML)
else
# limit txt targets to documents that have been prebuilt
txt: $(BUILD_DIR)/diff_v3_v4.txt $(BUILD_DIR)/MIGRATING.txt \
	$(BUILD_DIR)/INSTALL.txt $(BUILD_DIR)/INSTALL-win.txt \
	$(BUILD_DIR)/NEWS.txt $(BUILD_DIR)/CONTRIBUTING.txt
endif

all: man txt

# enable HTML pages build only from git repository
ifeq ($(wildcard ../.git),../.git)
ifneq ($(builddoc),p)
all: html
endif
endif

../version.inc:
	$(MAKE) --no-print-directory -C .. version.inc

# source version definitions shared across the Makefiles of this project
include ../version.inc

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

$(BUILD_DIR)/module.1.in $(BUILD_DIR)/modulefile.4 $(BUILD_DIR)/ml.1: $(ALL_RST) $(SOURCE_DIR)/version.py
	$(ECHO_GEN2) $(ECHO_DIR_PREFIX)$(BUILD_DIR)/ml.1
	$(ECHO_GEN2) $(ECHO_DIR_PREFIX)$(BUILD_DIR)/module.1.in
	$(ECHO_GEN2) $(ECHO_DIR_PREFIX)$(BUILD_DIR)/modulefile.4
	$(SPHINXBUILD) $(SPHINXOPTS) -t pathsubs -b man "$(SOURCE_DIR)" "$(BUILD_DIR)"
	$(if $(findstring module.1.in,$@),mv $(BUILD_DIR)/module.1 $@)
endif

$(BUILD_DIR)/module.1: $(BUILD_DIR)/module.1.in
	$(translate-in-script)

# compatibility version-related rules
$(COMPAT_MAN_DIR)/module.1 $(COMPAT_MAN_DIR)/modulefile.4:
	$(ECHO_GEN)
	$(MAKE) --no-print-directory -C $(COMPAT_MAN_DIR) $(@F)

install: man txt
	mkdir -p $(DESTDIR)$(mandir)/man1 $(DESTDIR)$(mandir)/man4
ifeq ($(docinstall),y)
	mkdir -p $(DESTDIR)$(docdir)
	cp $(BUILD_DIR)/diff_v3_v4.txt $(DESTDIR)$(docdir)/
	cp $(BUILD_DIR)/MIGRATING.txt $(DESTDIR)$(docdir)/
	cp $(BUILD_DIR)/INSTALL.txt $(DESTDIR)$(docdir)/
	cp $(BUILD_DIR)/INSTALL-win.txt $(DESTDIR)$(docdir)/
	cp $(BUILD_DIR)/NEWS.txt $(DESTDIR)$(docdir)/
	cp $(BUILD_DIR)/CONTRIBUTING.txt $(DESTDIR)$(docdir)/
	cp example.txt $(DESTDIR)$(docdir)/
endif
	cp $(BUILD_DIR)/module.1 $(DESTDIR)$(mandir)/man1/
	cp $(BUILD_DIR)/ml.1 $(DESTDIR)$(mandir)/man1/
	cp $(BUILD_DIR)/modulefile.4 $(DESTDIR)$(mandir)/man4/
ifeq ($(compatversion),y)
	cp $(COMPAT_MAN_DIR)/module.1 $(DESTDIR)$(mandir)/man1/module-compat.1
	cp $(COMPAT_MAN_DIR)/modulefile.4 $(DESTDIR)$(mandir)/man4/modulefile-compat.4
endif

uninstall:
ifeq ($(docinstall),y)
	rm -f $(DESTDIR)$(docdir)/diff_v3_v4.txt
	rm -f $(DESTDIR)$(docdir)/MIGRATING.txt
	rm -f $(DESTDIR)$(docdir)/INSTALL.txt
	rm -f $(DESTDIR)$(docdir)/INSTALL-win.txt
	rm -f $(DESTDIR)$(docdir)/NEWS.txt
	rm -f $(DESTDIR)$(docdir)/CONTRIBUTING.txt
	rm -f $(DESTDIR)$(docdir)/example.txt
	rmdir $(DESTDIR)$(docdir)
endif
	rm -f $(DESTDIR)$(mandir)/man1/module.1
	rm -f $(DESTDIR)$(mandir)/man1/ml.1
	rm -f $(DESTDIR)$(mandir)/man4/modulefile.4
ifeq ($(compatversion),y)
	rm -f $(DESTDIR)$(mandir)/man1/module-compat.1
	rm -f $(DESTDIR)$(mandir)/man4/modulefile-compat.4
endif
	rmdir $(DESTDIR)$(mandir)/man1 $(DESTDIR)$(mandir)/man4
	rmdir $(DESTDIR)$(mandir)

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
	$(BUILD_DIR)/module.1.in $(ALL_MAN) $(COMPAT_MAN_DIR)/module.1 \
	$(COMPAT_MAN_DIR)/modulefile.4
