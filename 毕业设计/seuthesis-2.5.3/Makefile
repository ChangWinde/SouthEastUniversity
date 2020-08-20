#
# Copyright (C) 2007 by Xu Yuan <xuyuan.cn@gmail.com>
# $Id$
#
# This file is part of the SEU-Thesis package project.
# ---------------------------------------------------
#
# This file may be distributed and/or modified under the
# conditions of the LaTeX Project Public License, either version 1.3a
# of this license or (at your option) any later version.
# The latest version of this license is in:
# 
# http://www.latex-project.org/lppl.txt
# 
# and version 1.3a or later is part of all distributions of LaTeX 
# version 2004/10/01 or later.
#

PACKAGE=seuthesis
SRC=${PACKAGE}.ins ${PACKAGE}.dtx

MAIN=main
MAIN_SRC=${MAIN}.tex content/*.tex content/reference.bib

# all: package

# main: main.pdf

# sample: sample.pdf

# package: ${PACKAGE}.pdf


clean:
	rm -f *.aux *.log *.toc *.ind *.inx *.gls *.glo *.idx *.ilg *.out *.bak *.bbl *.brf *.blg *.dvi *.ps *.gz
clean_all:
	rm -f *.aux *.log *.toc *.ind *.inx *.gls *.glo *.idx *.ilg *.out *.bak *.bbl *.brf *.blg *.dvi *.ps *.gz *.pdf

# distclean: clean
# 	rm -f *.cls *.cfg

# ${PACKAGE}.cls: ${SRC}
# 	rm -f ${PACKAGE}.cls ${PACKAGE}-gbk.cfg ${PACKAGE}-utf8.cfg
# 	latex ${PACKAGE}.ins
# 	iconv -f utf8 -t gbk ${PACKAGE}-utf8.cfg > ${PACKAGE}-gbk.cfg

# ${PACKAGE}.idx: ${PACKAGE}.dtx
# 	xelatex ${PACKAGE}.dtx

# ${PACKAGE}.bbl: ${PACKAGE}.dtx ${PACKAGE}.bib
# 	xelatex ${PACKAGE}.dtx
# 	bibtex ${PACKAGE}

# ${PACKAGE}.ind: ${PACKAGE}.idx
# 	makeindex -s gind ${PACKAGE}
# #	makeindex -s gglo -o ${PACKAGE}.gls ${PACKAGE}.glo

# ${PACKAGE}.pdf: ${PACKAGE}.dtx ${PACKAGE}.cls ${PACKAGE}.ind ${PACKAGE}.bbl
# 	xelatex ${PACKAGE}.dtx
# 	xelatex ${PACKAGE}.dtx

# sample.bbl: seuthesis.bib sample.tex
# 	xelatex sample
# 	bibtex sample

# sample.pdf: sample.tex ${PACKAGE}.cls sample.bbl
# 	xelatex sample
# 	xelatex sample

# # rules of making main (my thesis)
# main.bbl: main.tex content/reference.bib
# 	xelatex main
# 	bibtex -min-crossrefs=9000 main

# main.pdf: ${MAIN_SRC} ${PACKAGE}.cls main.bbl
# 	xelatex main
# 	xelatex main