JRE = java -classpath ../saxon.jar
XSLTPROC = com.icl.saxon.StyleSheet

HTMLDOCS = index.html resources.html

all: $(HTMLDOCS)

%.html: %.xml
	$(JRE) $(XSLTPROC) -a -o $@ $<

clean:
	$(RM) $(HTMLDOCS)

.PHONY: clean
