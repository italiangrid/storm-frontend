# Copyright (c) Istituto Nazionale di Fisica Nucleare (INFN). 2006-2010.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

EXTRA_DIST = srm.v2.2.wsdl typemap.dat

instdir = $(datadir)/wsdl
inst_DATA = srm.v2.2.wsdl
noinst_DATA = typemap.dat

# If this file exists means that the skeletons has been already generated
FLAG_FILE = skeletons_generated.lock

all: skeletons

dist-hook: skeletons

skeletons: $(FLAG_FILE)

$(FLAG_FILE):
	touch $(FLAG_FILE)
	mkdir -p $(top_builddir)/wsdl/gsoap-autogen
	@WSDL2H@ @WSDL2H_FLAGS@ -w -c -e -y -t $(top_srcdir)/wsdl/typemap.dat $(top_srcdir)/wsdl/srm.v2.2.wsdl -o $(top_builddir)/wsdl/gsoap-autogen/srm.v2.2.h
	cd $(top_builddir)/wsdl/gsoap-autogen; @SOAPCPP2@ -S -I/usr/include -c -p srmv2 srm.v2.2.h; mv srmv2C.c srmv2C.cc; mv srmv2Server.c srmv2Server.cc; cd ..

clean-local:
	rm -rf $(top_builddir)/wsdl/gsoap-autogen
	rm -f $(FLAG_FILE)
