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

# source version definitions shared across the Makefiles of this project
include ../version.inc

# quit if no sphinx-build found at configure time
ifneq ($(builddoc),y)
  $(error No 'sphinx-build' command defined, cannot build documentation)
endif

man: $(BUILD_DIR)/module.1 $(BUILD_DIR)/modulefile.4
ifeq ($(compatversion),y)
man: $(COMPAT_MAN_DIR)/module.1 $(COMPAT_MAN_DIR)/modulefile.4
endif

txt: $(BUILD_DIR)/diff_v3_v4.txt $(BUILD_DIR)/MIGRATING.txt \
	$(BUILD_DIR)/INSTALL.txt $(BUILD_DIR)/NEWS.txt \
	$(BUILD_DIR)/CONTRIBUTING.txt

html: $(BUILD_DIR)/module.html $(BUILD_DIR)/modulefile.html \
	$(BUILD_DIR)/diff_v3_v4.html $(BUILD_DIR)/MIGRATING.html \
	$(BUILD_DIR)/INSTALL.html $(BUILD_DIR)/NEWS.html \
	$(BUILD_DIR)/CONTRIBUTING.html

all: man txt

# enable HTML pages build only from git repository
ifeq ($(wildcard ../.git),../.git)
all: html
endif

define translate-in-script
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
../Makefile.inc ../version.inc: ;

# build doc from source if in git repository
ifeq ($(wildcard ../.git),../.git)
$(SOURCE_DIR)/version.py: $(SOURCE_DIR)/version.py.in ../version.inc
	$(translate-in-script)

$(BUILD_DIR)/%.html: $(SOURCE_DIR)/%.rst $(SOURCE_DIR)/version.py
	$(SPHINXBUILD) $(SPHINXOPTS) -b html "$(SOURCE_DIR)" "$(BUILD_DIR)"

$(BUILD_DIR)/%.txt: $(SOURCE_DIR)/%.rst $(SOURCE_DIR)/version.py
	$(SPHINXBUILD) $(SPHINXOPTS) -b text "$(SOURCE_DIR)" "$(BUILD_DIR)"

$(BUILD_DIR)/%.4: $(SOURCE_DIR)/%.rst $(SOURCE_DIR)/version.py
	$(SPHINXBUILD) $(SPHINXOPTS) -t pathsubs -b man "$(SOURCE_DIR)" "$(BUILD_DIR)"

$(BUILD_DIR)/%.1.in: $(SOURCE_DIR)/%.rst $(SOURCE_DIR)/version.py
	$(SPHINXBUILD) $(SPHINXOPTS) -t pathsubs -b man "$(SOURCE_DIR)" "$(BUILD_DIR)"
	mv $(BUILD_DIR)/module.1 $@
endif

$(BUILD_DIR)/module.1: $(BUILD_DIR)/module.1.in
	$(translate-in-script)

# compatibility version-related rules
$(COMPAT_MAN_DIR)/module.1 $(COMPAT_MAN_DIR)/modulefile.4:
	$(MAKE) -C $(COMPAT_MAN_DIR) $(@F)

install: man txt
	mkdir -p $(DESTDIR)$(mandir)/man1 $(DESTDIR)$(mandir)/man4
ifeq ($(docinstall),y)
	mkdir -p $(DESTDIR)$(docdir)
	cp $(BUILD_DIR)/diff_v3_v4.txt $(DESTDIR)$(docdir)/
	cp $(BUILD_DIR)/MIGRATING.txt $(DESTDIR)$(docdir)/
	cp $(BUILD_DIR)/INSTALL.txt $(DESTDIR)$(docdir)/
	cp $(BUILD_DIR)/NEWS.txt $(DESTDIR)$(docdir)/
	cp $(BUILD_DIR)/CONTRIBUTING.txt $(DESTDIR)$(docdir)/
	cp example.txt $(DESTDIR)$(docdir)/
endif
	cp $(BUILD_DIR)/module.1 $(DESTDIR)$(mandir)/man1/
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
	rm -f $(DESTDIR)$(docdir)/NEWS.txt
	rm -f $(DESTDIR)$(docdir)/CONTRIBUTING.txt
	rm -f $(DESTDIR)$(docdir)/example.txt
	rmdir $(DESTDIR)$(docdir)
endif
	rm -f $(DESTDIR)$(mandir)/man1/module.1
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
	rm -f $(BUILD_DIR)/*.1
