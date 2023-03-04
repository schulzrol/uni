ALL=$(wildcard *.tex)
PAPER=proj1.pdf
SHELL=/bin/sh

TRASH=$(wildcard *.log *.aux *.fls *.out *.fdb_latexmk)

all: clean
	pdflatex ${ALL} --output ${PAPER}

clean:
	rm -f ${TRASH}
	rm -f ${PAPER}
