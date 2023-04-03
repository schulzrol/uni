SRCS=proj3.tex
PAPER=proj3.pdf
SHELL=/bin/sh
VZOR_PAPER=$(wildcard vzor-*.pdf)
TRASH=$(wildcard *.blg *.bbl *.log *.aux *.fls *.out *.fdb_latexmk *.te~ *.synctex.gz *.dvi *.ps)
DIFF_FILE=diff.pdf


.PHONY: all clean diff

all: 
#double compilation for correct reference numbers
	latex ${SRCS}
	pdflatex ${SRCS} --output ${PAPER}

vlna: ${SRCS}
	vlna $^

diff:
	pdftk ${VZOR_PAPER} multibackground ${PAPER} output ${DIFF_FILE}

clean:
	rm -f ${TRASH}
	rm -f ${PAPER}
	rm -f ${DIFF_FILE}
