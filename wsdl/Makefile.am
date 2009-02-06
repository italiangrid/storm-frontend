# $Id: Makefile.am 4399 2008-03-14 11:00:26Z alb $

#wsdldir = ${prefix}/wsdl
#wsdl_DATA = srm.v2.2.wsdl
#noinst_DATA = typemap.dat

EXTRA_DIST = srm.v2.2.wsdl typemap.dat

instdir = ${prefix}/wsdl
inst_DATA = srm.v2.2.wsdl
noinst_DATA = typemap.dat

# If this file exists means that the skeletons has been already generated
FLAG_FILE = skeletons_generated.lock

all: skeletons

dist-hook: skeletons

skeletons: $(FLAG_FILE)

$(FLAG_FILE):
	touch $(FLAG_FILE)
	mkdir -p @GSOAP_GENERATED_CODE_DIR_NAME@
	@GSOAP_DIR@/bin/wsdl2h -e -y -c -t $(top_srcdir)/wsdl/typemap.dat $(top_srcdir)/wsdl/srm.v2.2.wsdl -o ./@GSOAP_GENERATED_CODE_DIR_NAME@/srm.v2.2.h
	cd @GSOAP_GENERATED_CODE_DIR_NAME@; @GSOAP_DIR@/bin/soapcpp2 -S -x -w -c -p srmv2 srm.v2.2.h; cd ..

# Note: if with an upgraded version of gSOAP doesn't compile, try to add option -z in "wsdl2h"

clean-local:
	rm -rf @GSOAP_GENERATED_CODE_DIR_NAME@
	rm -f $(FLAG_FILE)

