.PHONY: install uninstall clean

HTML_DIR := html
MAN_DIR := man
MAN1_DIR := $(MAN_DIR)/man1
MAN4_DIR := $(MAN_DIR)/man4
SOURCE_DIR := source

# compatibility version-related files
COMPAT_MAN_DIR := ../compat/doc

# source definitions shared across the Makefiles of this project
ifneq ($(wildcard ../Makefile.inc),../Makefile.inc)
  $(error ../Makefile.inc is missing, please run '../configure')
endif
include ../Makefile.inc

# source version definitions shared across the Makefiles of this project
include ../version.inc

MAN_VERSION := "modules $(MODULES_RELEASE)$(MODULES_BUILD)"
MAN_HEADER := "Environment Modules"

ifeq ($(compatversion),y)
man: $(MAN1_DIR)/module.1 $(MAN4_DIR)/modulefile.4 \
	$(COMPAT_MAN_DIR)/module.1 $(COMPAT_MAN_DIR)/modulefile.4
else
man: $(MAN1_DIR)/module.1 $(MAN4_DIR)/modulefile.4
endif

txt: diff_v3_v4.txt

html: $(HTML_DIR)/module.html $(HTML_DIR)/modulefile.html \
	$(HTML_DIR)/diff_v3_v4.html

all: html man txt

define translate-in-script
perl -pe 's|\@prefix\@|$(prefix)|g; \
	s|\@libexecdir\@|$(libexecdir)|g; \
	s|\@initdir\@|$(initdir)|g; \
	s|\@modulefilesdir\@|$(modulefilesdir)|g;' $< > $@
endef

# avoid shared definitions to be rebuilt by make
../Makefile.inc ../version.inc: ;

$(SOURCE_DIR)/%.pod: $(SOURCE_DIR)/%.pod.in
	$(translate-in-script)

$(HTML_DIR)/%.html: $(SOURCE_DIR)/%.pod
	mkdir -p $(HTML_DIR)
	pod2html --noindex --css=common.css --infile=$< --outfile=$@
	rm -f pod2htmd.tmp

$(MAN1_DIR)/%.1: $(SOURCE_DIR)/%.pod ../version.inc
	mkdir -p $(MAN1_DIR)
	pod2man -c $(MAN_HEADER) -r $(MAN_VERSION) -s 1 $< $@

$(MAN4_DIR)/%.4: $(SOURCE_DIR)/%.pod ../version.inc
	mkdir -p $(MAN4_DIR)
	pod2man -c $(MAN_HEADER) -r $(MAN_VERSION) -s 4 $< $@

%.txt: $(SOURCE_DIR)/%.pod
	pod2text $< >$@

# compatibility version-related rules
$(COMPAT_MAN_DIR)/module.1 $(COMPAT_MAN_DIR)/modulefile.4:
	make -C $(COMPAT_MAN_DIR) $(@F)

install: all
	mkdir -p $(DESTDIR)$(mandir)/man1 $(DESTDIR)$(mandir)/man4
ifeq ($(docinstall),y)
	mkdir -p $(DESTDIR)$(docdir)
	cp diff_v3_v4.txt $(DESTDIR)$(docdir)/
	cp example.txt $(DESTDIR)$(docdir)/
endif
	cp $(MAN1_DIR)/module.1 $(DESTDIR)$(mandir)/man1/
	cp $(MAN4_DIR)/modulefile.4 $(DESTDIR)$(mandir)/man4/
ifeq ($(compatversion),y)
	cp $(COMPAT_MAN_DIR)/module.1 $(DESTDIR)$(mandir)/man1/module-compat.1
	cp $(COMPAT_MAN_DIR)/modulefile.4 $(DESTDIR)$(mandir)/man4/modulefile-compat.4
endif

uninstall:
ifeq ($(docinstall),y)
	rm -f $(DESTDIR)$(docdir)/diff_v3_v4.txt
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
	rm -f diff_v3_v4.txt
	rm -f pod*.tmp
