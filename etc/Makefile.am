storm_etc_logrotate_dir = $(sysconfdir)/logrotate
storm_etc_logrotate_dir_DATA = ./logrotate/storm-frontend.cron ./logrotate/storm-frontend.logrotate

EXTRA_DIST = $(storm_etc_logrotate_dir_DATA)

install-data-hook: $(storm_etc_logrotate_dir_DATA)
	if [ ! -d $(DESTDIR)$(storm_etc_logrotate_dir) ]; then \
		$(mkinstalldirs) $(DESTDIR)$(storm_etc_logrotate_dir); \
	fi
	for file in $(storm_etc_logrotate_dir_DATA); do \
		/bin/cp $$file $(DESTDIR)$(storm_etc_logrotate_dir)/`basename $$file`; \
	done

