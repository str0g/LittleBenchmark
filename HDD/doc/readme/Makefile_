docdir = $(datadir)/doc/@PACKAGE@
doc_DATA = readme.pdf

all: clean readme.pdf

pdf: readme.pdf

readme.pdf: readme.tex
	pdflatex readme.tex
	makeindex readme.idx
	pdflatex readme.tex
	latex_count=5 ; \
	while egrep -s 'Rerun (LaTeX|to get cross-references right)' readme.log && [ $$latex_count -gt 0 ] ;\
	    do \
	      echo "Rerunning latex...." ;\
	      pdflatex readme.tex ;\
	      latex_count=`expr $$latex_count - 1` ;\
	    done


clean:
	rm -f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out readme.pdf
