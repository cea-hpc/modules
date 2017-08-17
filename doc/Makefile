.PHONY: install uninstall clean

VERSION_FILE := ../modulecmd.tcl.in
MAN_VERSION := "modules-tcl $(shell grep '^set MODULES_CURRENT_VERSION' \
	$(VERSION_FILE) | cut -d ' ' -f 3)"
MAN_HEADER := "Environment Modules"

HTML_DIR := html
MAN_DIR := man
MAN1_DIR := $(MAN_DIR)/man1
MAN4_DIR := $(MAN_DIR)/man4
SOURCE_DIR := source

# source definitions shared across the Makefiles of this project
ifneq ($(wildcard ../Makefile.inc),../Makefile.inc)
  $(error ../Makefile.inc is missing, please run '../configure')
endif
include ../Makefile.inc

man: $(MAN1_DIR)/module.1 $(MAN4_DIR)/modulefile.4

txt: diff_with_c-version.txt

html: $(HTML_DIR)/module.html $(HTML_DIR)/modulefile.html \
	$(HTML_DIR)/diff_with_c-version.html

all: html man txt

define translate-in-script
perl -pe 's|\@prefix\@|$(prefix)|g; \
	s|\@libexecdir\@|$(libexecdir)|g; \
	s|\@initdir\@|$(initdir)|g; \
	s|\@modulefilesdir\@|$(modulefilesdir)|g;' $< > $@
endef

$(SOURCE_DIR)/%.pod: $(SOURCE_DIR)/%.pod.in
	$(translate-in-script)

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
	mkdir -p $(DESTDIR)$(mandir)/man1 $(DESTDIR)$(mandir)/man4
ifeq ($(docinstall),y)
	mkdir -p $(DESTDIR)$(docdir)
	cp diff_with_c-version.txt $(DESTDIR)$(docdir)/
	cp example.txt $(DESTDIR)$(docdir)/
endif
	cp $(MAN1_DIR)/module.1 $(DESTDIR)$(mandir)/man1/
	cp $(MAN4_DIR)/modulefile.4 $(DESTDIR)$(mandir)/man4/

uninstall:
ifeq ($(docinstall),y)
	rm -f $(DESTDIR)$(docdir)/diff_with_c-version.txt
	rm -f $(DESTDIR)$(docdir)/example.txt
	rmdir $(DESTDIR)$(docdir)
endif
	rm -f $(DESTDIR)$(mandir)/man1/module.1
	rm -f $(DESTDIR)$(mandir)/man4/modulefile.4
	rmdir $(DESTDIR)$(mandir)/man1 $(DESTDIR)$(mandir)/man4
	rmdir $(DESTDIR)$(mandir)

clean:
	rm -f $(HTML_DIR)/*.html
	rm -f $(MAN1_DIR)/*.1 $(MAN4_DIR)/*.4
	rm -f $(SOURCE_DIR)/module.pod
ifneq ($(wildcard $(MAN1_DIR)),)
	rmdir $(MAN1_DIR)
endif
ifneq ($(wildcard $(MAN4_DIR)),)
	rmdir $(MAN4_DIR)
endif
ifneq ($(wildcard $(MAN_DIR)),)
	rmdir $(MAN_DIR)
endif
	rm -f diff_with_c-version.txt
	rm -f pod*.tmp
