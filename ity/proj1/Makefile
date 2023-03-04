SRCS=$(wildcard *.tex)
PAPER=proj1.pdf
SHELL=/bin/sh
TRASH=$(wildcard *.log *.aux *.fls *.out *.fdb_latexmk *.te~ *.synctex.gz)

.PHONY: all clean

all: clean 
	pdflatex ${SRCS} --output ${PAPER}

vlna: ${SRCS}
	vlna $^

clean:
	rm -f ${TRASH}
	rm -f ${PAPER}
