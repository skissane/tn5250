XSLTPROC := xsltproc

LASTMOD = $(shell date -u -r $<)

HTMLDOCS := index.html resources.html

all: $(HTMLDOCS)

$(HTMLDOCS): tn5250.xsl

%.html: %.xml
	$(XSLTPROC) --stringparam lastmod "$(LASTMOD)" -o $@ $<

clean:
	$(RM) $(HTMLDOCS)

.PHONY: all clean
