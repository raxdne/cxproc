## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libxmlzipio.la

libxmlzipio_la_SOURCES = xmlzipio.c zip.c unzip.c
libxmlzipio_la_CFLAGS = -Wall -Wuninitialized -O3 \
	`xml2-config --cflags` -DDEF_MEM_LEVEL=8
# -DXMLZIPIO_VERBOSE

libxmlzipio_la_LDFLAGS = -lz 

README: xmlzipio.html
	html2text $< > $@

TARBALLFILES = zip.c unzip.c zip.h unzip.h xmlzipio.c xmlzipio.h Makefile.am \
	README LICENSE xmlzipio.html schraubstock.jpg

xmlzipio.tar.gz: $(TARBALLFILES)
	tar cvfz $@ $(TARBALLFILES)
