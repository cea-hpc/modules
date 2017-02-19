.PHONY: install clean .makeinstallpath

VERSION_FILE := ../modulecmd.tcl
MAN_VERSION := "modules-tcl $(shell grep '^set MODULES_CURRENT_VERSION' \
	$(VERSION_FILE) | cut -d ' ' -f 3)"
MAN_HEADER := "Environment Modules"

HTML_DIR := html
MAN_DIR := man
MAN1_DIR := $(MAN_DIR)/man1
MAN4_DIR := $(MAN_DIR)/man4
SOURCE_DIR := source

# load previously saved install paths if any
-include .makeinstallpath

# set default installation paths if not yet defined
prefix ?= /usr/local/modules-tcl
datarootdir ?= $(prefix)/share
mandir ?= $(datarootdir)/man
docdir ?= $(datarootdir)/doc

all: $(HTML_DIR)/module.html $(HTML_DIR)/modulefile.html \
	$(MAN1_DIR)/module.1 $(MAN4_DIR)/modulefile.4 \
	$(HTML_DIR)/diff_with_c-version.html diff_with_c-version.txt \
	.makeinstallpath

# save defined install paths
.makeinstallpath:
	@echo "prefix := $(prefix)" >$@
	@echo "datarootdir := $(datarootdir)" >>$@
	@echo "mandir := $(mandir)" >>$@
	@echo "docdir := $(docdir)" >>$@

$(HTML_DIR)/%.html: $(SOURCE_DIR)/%.pod $(VERSION_FILE)
	mkdir -p $(HTML_DIR)
	pod2html --noindex --css=common.css --infile=$< --outfile=$@
	rm -f pod2htmd.tmp

$(MAN1_DIR)/%.1: $(SOURCE_DIR)/%.pod $(VERSION_FILE)
	mkdir -p $(MAN1_DIR)
	pod2man -c $(MAN_HEADER) -r $(MAN_VERSION) -s 1 $< $@

$(MAN4_DIR)/%.4: $(SOURCE_DIR)/%.pod $(VERSION_FILE)
	mkdir -p $(MAN4_DIR)
	pod2man -c $(MAN_HEADER) -r $(MAN_VERSION) -s 4 $< $@

%.txt: $(SOURCE_DIR)/%.pod $(VERSION_FILE)
	pod2text $< >$@

install: all
	mkdir -p $(docdir)
	mkdir -p $(mandir)/man1 $(mandir)/man4
	cp LICENSE.GPL $(docdir)/
	cp diff_with_c-version.txt $(docdir)/
	cp $(MAN1_DIR)/module.1 $(mandir)/man1/
	cp $(MAN4_DIR)/modulefile.4 $(mandir)/man4/

clean:
	rm -f $(HTML_DIR)/*.html
	rm -f $(MAN1_DIR)/*.1 $(MAN4_DIR)/*.4
	rmdir $(MAN1_DIR) $(MAN4_DIR) $(MAN_DIR)
	rm -f diff_with_c-version.txt
	rm -f pod*.tmp .makeinstallpath
