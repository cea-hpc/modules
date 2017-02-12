.PHONY: clean

TCL_DIR := tcl
DOCS_DIR := docs
TOP_DIR := ..
POD_SRC_DIR := $(TOP_DIR)/doc/source
PAPER_SRC_DIR := $(TOP_DIR)/doc/paper
TALK_SRC_DIR := $(TOP_DIR)/doc/talk
TXT_SRC_DIR := $(TOP_DIR)/doc

all: $(TCL_DIR)/module.html $(TCL_DIR)/modulefile.html \
	$(TCL_DIR)/diff_with_c-version.html $(TCL_DIR)/NEWS.html \
    $(DOCS_DIR)/example.txt $(DOCS_DIR)/CSE-IT_modules_talk.pdf \
	$(DOCS_DIR)/absmod.pdf $(DOCS_DIR)/Modules-Paper.pdf \
	$(DOCS_DIR)/MC2_whitney_paper.pdf $(DOCS_DIR)/user-setup.pdf

$(TCL_DIR)/%.html: $(POD_SRC_DIR)/%.pod
	mkdir -p $(TCL_DIR)
	pod2html --noindex --infile=$< --outfile=$@
	rm -f pod2htmd.tmp
	../contrib/doc2www $@

$(TCL_DIR)/NEWS.html: $(TOP_DIR)/NEWS
	mkdir -p $(TCL_DIR)
	cp $< $@
	../contrib/doc2www $@

$(DOCS_DIR)/%.txt: $(TXT_SRC_DIR)/%.txt
	mkdir -p $(DOCS_DIR)
	cp $< $@

$(DOCS_DIR)/%.pdf: $(PAPER_SRC_DIR)/%.pdf
	mkdir -p $(DOCS_DIR)
	cp $< $@

$(DOCS_DIR)/%.pdf: $(TALK_SRC_DIR)/%.pdf
	mkdir -p $(DOCS_DIR)
	cp $< $@

clean:
	rm -f $(TCL_DIR)/*.html
	rm -f $(DOCS_DIR)/*.pdf $(DOCS_DIR)/*.txt
	rm -f pod*.tmp
