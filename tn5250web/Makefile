JRE = java -classpath ../saxon.jar
XSLTPROC = com.icl.saxon.StyleSheet

LASTMOD = $(shell date -u -r $<)

HTMLDOCS = index.html resources.html

all: $(HTMLDOCS)

$(HTMLDOCS): tn5250.xsl

%.html: %.xml
	$(JRE) $(XSLTPROC) -a -o $@.tmp $< lastmod="$(LASTMOD)" && mv $@.tmp $@

clean:
	$(RM) $(HTMLDOCS)

.PHONY: clean
