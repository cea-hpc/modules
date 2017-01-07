.PHONY: clean

VERSION_FILE := ../modulecmd.tcl
MAN_VERSION := "modules-tcl $(shell grep '^set MODULES_CURRENT_VERSION' \
	$(VERSION_FILE) | cut -d ' ' -f 3)"
MAN_HEADER := "Environment Modules"

HTML_DIR := html
MAN_DIR := man
MAN1_DIR := $(MAN_DIR)/man1
MAN4_DIR := $(MAN_DIR)/man4
SOURCE_DIR := source

all: $(HTML_DIR)/module.html $(HTML_DIR)/modulefile.html \
	$(MAN1_DIR)/module.1 $(MAN4_DIR)/modulefile.4

$(HTML_DIR)/%.html: $(SOURCE_DIR)/%.pod $(VERSION_FILE)
	mkdir -p $(HTML_DIR)
	pod2html --noindex --infile=$< --outfile=$@
	rm -f pod2htmd.tmp

$(MAN1_DIR)/%.1: $(SOURCE_DIR)/%.pod $(VERSION_FILE)
	mkdir -p $(MAN1_DIR)
	pod2man -c $(MAN_HEADER) -r $(MAN_VERSION) -s 1 $< $@

$(MAN4_DIR)/%.4: $(SOURCE_DIR)/%.pod $(VERSION_FILE)
	mkdir -p $(MAN4_DIR)
	pod2man -c $(MAN_HEADER) -r $(MAN_VERSION) -s 4 $< $@

clean:
	rm -f $(HTML_DIR)/*.html
	rm -f $(MAN1_DIR)/*.1 $(MAN4_DIR)/*.4
	rmdir $(HTML_DIR)
	rmdir $(MAN1_DIR) $(MAN4_DIR) $(MAN_DIR)
	rm -f pod*.tmp
