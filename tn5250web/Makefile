JRE = java -classpath ../saxon.jar
XSLTPROC = com.icl.saxon.StyleSheet

TZ=UTC

HTMLDOCS = index.html resources.html

all: $(HTMLDOCS)

$(HTMLDOCS): tn5250.xsl

%.html: %.xml
	$(JRE) $(XSLTPROC) -a -o $@.tmp $< lastmod="`date -r $<`" && mv $@.tmp $@

clean:
	$(RM) $(HTMLDOCS)

.PHONY: clean
