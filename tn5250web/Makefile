JRE = java -classpath ../saxon.jar
XSLTPROC = com.icl.saxon.StyleSheet

HTMLDOCS = index.html resources.html

all: $(HTMLDOCS)

$(HTMLDOCS): tn5250.xsl

%.html: %.xml
	$(JRE) $(XSLTPROC) -a -o $@.tmp $< && mv $@.tmp $@

clean:
	$(RM) $(HTMLDOCS)

.PHONY: clean
