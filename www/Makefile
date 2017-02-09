.PHONY: clean

TCL_DIR := tcl
DOCS_DIR := docs
POD_SRC_DIR := ../doc/source
PAPER_SRC_DIR := ../doc/paper
TALK_SRC_DIR := ../doc/talk
TXT_SRC_DIR := ../doc

all: $(TCL_DIR)/module.html $(TCL_DIR)/modulefile.html \
	$(TCL_DIR)/diff_with_c-version.html $(DOCS_DIR)/example.txt \
	$(DOCS_DIR)/absmod.pdf $(DOCS_DIR)/Modules-Paper.pdf \
	$(DOCS_DIR)/MC2_whitney_paper.pdf $(DOCS_DIR)/user-setup.pdf \
	$(DOCS_DIR)/CSE-IT_modules_talk.pdf

$(TCL_DIR)/%.html: $(POD_SRC_DIR)/%.pod
	mkdir -p $(TCL_DIR)
	pod2html --noindex --infile=$< --outfile=$@
	rm -f pod2htmd.tmp
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
