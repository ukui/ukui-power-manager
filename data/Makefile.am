SUBDIRS = icons


@INTLTOOL_DESKTOP_RULE@
desktopdir = $(datadir)/applications
desktop_in_files = ukui-power-preferences.desktop.in \
		   ukui-power-statistics.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@
autostartdir = $(sysconfdir)/xdg/autostart
autostart_in_files = ukui-power-manager.desktop.in
autostart_DATA = $(autostart_in_files:.desktop.in=.desktop)

servicedir       = $(DBUS_SERVICES_DIR)
service_in_files = org.ukui.PowerManager.service.in
service_DATA     = $(service_in_files:.service.in=.service)

$(service_DATA): $(service_in_files) Makefile
	@sed -e "s|\@servicedir\@|$(bindir)|" $< > $@

@GSETTINGS_RULES@
@INTLTOOL_XML_NOMERGE_RULE@
gsettings_schemas_in_files = org.ukui.power-manager.gschema.xml.in
gsettings_SCHEMAS = $(gsettings_schemas_in_files:.xml.in=.xml)

#kobe
#confdir = $(sysconfdir)/polkit-1/localauthority/50-local.d
#conf_DATA = com.ubuntu.enable-hibernate.pkla

pkgdata_DATA =						\
	gpm-statistics.ui				\
	gpm-feedback-widget.ui			\
	gpm-prefs.ui					\
	acme.ui

man_MANS =				\
	ukui-power-manager.1		\
	ukui-power-manager-tray.1	\
	ukui-power-backlight-helper.1	\
	ukui-power-statistics.1		\
	ukui-power-preferences.1

EXTRA_DIST =						\
	$(service_in_files)				\
	$(autostart_in_files)				\
	$(desktop_in_files)				\
	$(gsettings_schemas_in_files)	\
	$(pkgdata_DATA)			\
	$(man_MANS)
#	$(conf_DATA)

clean-local :
	rm -f *~

DISTCLEANFILES =					\
	ukui-power-manager.desktop			\
	ukui-power-preferences.desktop			\
	ukui-power-statistics.desktop			\
	org.ukui.PowerManager.service			\
	$(gsettings_SCHEMAS)
