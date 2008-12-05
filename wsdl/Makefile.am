# $Id: Makefile.am 4399 2008-03-14 11:00:26Z alb $

dist_data_DATA = typemap.dat srm.v2.2.wsdl

all: skeletons

dist-hook: skeletons

skeletons: ./@GSOAP_GENERATED_CODE_DIR_NAME@/srm.v2.2.h

./@GSOAP_GENERATED_CODE_DIR_NAME@/srm.v2.2.h:
	mkdir -p @GSOAP_GENERATED_CODE_DIR_NAME@
	@GSOAP_DIR@/wsdl2h -e -z -c -t $(top_srcdir)/wsdl/typemap.dat $(top_srcdir)/wsdl/srm.v2.2.wsdl -o ./@GSOAP_GENERATED_CODE_DIR_NAME@/srm.v2.2.h
	cd @GSOAP_GENERATED_CODE_DIR_NAME@; @GSOAP_DIR@/soapcpp2 -S -c -p srmv2 srm.v2.2.h; cd ..

clean-local:
	rm -rf @GSOAP_GENERATED_CODE_DIR_NAME@
