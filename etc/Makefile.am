storm_etc_logrotatedir = $(sysconfdir)/logrotate
storm_etc_logrotate_DATA = ./logrotate/storm-frontend.cron ./logrotate/storm-frontend.logrotate

EXTRA_DIST = $(storm_etc_logrotate_DATA)

install-data-hook: $(storm_etc_logrotate_DATA)
	if [ ! -d $(DESTDIR)$(storm_etc_logrotatedir) ]; then \
		$(mkinstalldirs) $(DESTDIR)$(storm_etc_logrotatedir); \
	fi
	for file in $(storm_etc_logrotate_DATA); do \
		/bin/cp $$file $(DESTDIR)$(storm_etc_logrotatedir)/`basename $$file`; \
	done

