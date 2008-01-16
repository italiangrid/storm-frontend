# $Id$
#*********************************************************************
#*
#* Authors: Flavia Donno <Flavia.Donno@cern.ch>
#*
#* Copyright 2004 INFN FIRB Project
#*           http://grid-it.cnaf.infn.it/
#*
#*
#* Date: 01 March 2005
#*
#*********************************************************************

EXTRA_DIST = srmv2-storm-server.spec.in frontend-setup.sh frontend-setup.csh \
             clean.sh.in  autogen.sh LICENSE README VERSION

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 \
                       configure config.h.in \
                       stamp-h.in stamp-h1

FESERV_SPEC = srmv2-storm-server.spec

SUBDIRS = db srmv2

STORM_SPEC = $(FESERV_SPEC)

RPM_SPEC_PATH=@RPM_DIR@/SPECS

rpm: $(STORM_SPEC)
	gmake -i dist; \
	/bin/mkdir -p @RPM_DIR@/SOURCES \
                      @RPM_DIR@/SPECS   \
                      @RPM_DIR@/BUILD   \
                      @RPM_DIR@/RPMS    \
                      @RPM_DIR@/SRPMS; \
	/bin/cp -u @PACKAGE@-@VERSION@.tar.gz \
                      @RPM_DIR@/SOURCES; \
	for file in  $(STORM_SPEC) ; do \
	  /bin/cp $$file $(RPM_SPEC_PATH); \
	done; \
	cd $(RPM_SPEC_PATH); \
	for file in $(STORM_SPEC) ; do \
	   /usr/bin/rpmbuild  --define "_topdir @RPM_DIR@" -ba $$file; \
	done

dist-hook:
	cd $(distdir)
	for file in $(MAINTAINERCLEANFILES); do \
	  /bin/rm -f $(distdir)/$$file; \
	done


clean-tree:
	chmod a+rx ./clean.sh
	./clean.sh

