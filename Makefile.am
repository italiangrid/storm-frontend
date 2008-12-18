# $Id$

SUBDIRS = etc wsdl src

SPEC_FILE = storm-frontend.spec

RPM_DIRS = @RPM_DIR@/SOURCES @RPM_DIR@/SPECS @RPM_DIR@/BUILD @RPM_DIR@/RPMS @RPM_DIR@/SRPMS

rpm: $(SPEC_FILE) \
     dist
	/bin/mkdir -p $(RPM_DIRS)
	/bin/cp -u @PACKAGE@-@VERSION@.tar.gz @RPM_DIR@/SOURCES
	/bin/cp $(SPEC_FILE) @RPM_DIR@/SPECS
	cd $(RPM_SPEC_PATH); \
        /usr/bin/rpmbuild  --define "_topdir @RPM_DIR@" -ba @RPM_DIR@/SPECS/$(SPEC_FILE)

install-data-hook:
	mkdir -p $(DESTDIR)/etc/cron.d
	mv $(DESTDIR)/${prefix}/etc/cron.d/storm-frontend.cron $(DESTDIR)/etc/cron.d
	mkdir -p $(DESTDIR)/etc/init.d
	mv $(DESTDIR)/${prefix}/etc/init.d/storm-frontend $(DESTDIR)/etc/init.d

uninstall-local:
	rm -f $(DESTDIR)/etc/cron.d/storm-frontend.cron
	rm -f $(DESTDIR)/etc/init.d/storm-frontend

clean-local:
	rm -f @PACKAGE@-@VERSION@.tar.gz

distclean-local:
	rm -rf $(RPM_DIRS)
