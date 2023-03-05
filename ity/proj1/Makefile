SRCS=$(wildcard *.tex)
PAPER=proj1.pdf
SHELL=/bin/sh
TRASH=$(wildcard *.blg *.bbl *.log *.aux *.fls *.out *.fdb_latexmk *.te~ *.synctex.gz *.dvi *.ps)


.PHONY: all clean


all: 
#double compilation for correct reference numbers
	latex ${SRCS}
	pdflatex ${SRCS} --output ${PAPER}

vlna: ${SRCS}
	vlna $^

clean:
	rm -f ${TRASH}
	rm -f ${PAPER}
