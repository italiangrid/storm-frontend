# $Id$

SUBDIRS = etc wsdl src

SPEC_FILE = srmv2-storm-server.spec

rpm: $(SPEC_FILE) \
     dist
	/bin/mkdir -p @RPM_DIR@/SOURCES \
                      @RPM_DIR@/SPECS   \
                      @RPM_DIR@/BUILD   \
                      @RPM_DIR@/RPMS    \
                      @RPM_DIR@/SRPMS
	/bin/cp -u @PACKAGE@-@VERSION@.tar.gz @RPM_DIR@/SOURCES
	/bin/cp $(SPEC_FILE) @RPM_DIR@/SPECS
	cd $(RPM_SPEC_PATH); \
        /usr/bin/rpmbuild  --define "_topdir @RPM_DIR@" -ba @RPM_DIR@/SPECS/$(SPEC_FILE)

