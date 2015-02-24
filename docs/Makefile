PUBTEX := proposal.tex
PUBBIB := ref.bib
PUBPDF := $(PUBTEX:%.tex=%.pdf)

all: $(PUBPDF)

.SUFFIXES: .aux .bbl .blg .dvi .log
PDFLATEX_OPTS := -interaction=nonstopmode -halt-on-error -file-line-error -shell-escape

$(PUBPDF): $(PUBTEX) $(PUBDEPS) $(PUBBIB)
	pdflatex -draftmode $(PDFLATEX_OPTS) $(@:%.pdf=%)
	bibtex $(@:%.pdf=%)
	pdflatex -draftmode $(PDFLATEX_OPTS) $(@:%.pdf=%)
	pdflatex $(PDFLATEX_OPTS) $(@:%.pdf=%)

clean:
	rm -rf *.log *.aux *.blg *.bbl
	$(RM) *.dep *.dpth *.figlist *.makefile

distclean: clean
	rm -rf *.pdf

view: $(PUBPDF)
	evince $(PUBPDF) &

.PHONY: all clean distclean view

