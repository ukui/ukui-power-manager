AM_CPPFLAGS =							\
	-I. -I$(srcdir) 					\
	$(GLIB_CFLAGS)						\
	$(DBUS_CFLAGS)						\
	$(CAIRO_CFLAGS)						\
	$(PANEL_CFLAGS)						\
	$(LIBNOTIFY_CFLAGS)					\
	-DBINDIR=\"$(bindir)\"			 		\
	-DUKUILOCALEDIR=\""$(datadir)/locale"\"		\
	-DDATADIR=\"$(datadir)\"				\
	-DPREFIX=\""$(prefix)"\" 				\
	-DSYSCONFDIR=\""$(sysconfdir)"\" 			\
	-DLIBDIR=\""$(libdir)"\" 				\
	-DVERSION="\"$(VERSION)\"" 				\
	-DGPM_DATA=\"$(pkgdatadir)\"				\
	-DINHIBIT_MENU_UI_DIR=\""$(uidir)"\"			\
	-DEGG_LOG_FILE=\""~/ukui-power-manager.log"\"		\
	-DEGG_VERBOSE="\"GPM_VERBOSE\""				\
	-DEGG_LOGGING="\"GPM_LOGGING\""				\
	-DEGG_CONSOLE="\"GPM_CONSOLE\""				\
	$(DISABLE_DEPRECATED)					\
	$(NULL)

libexec_PROGRAMS=ukui-inhibit-applet

ukui_inhibit_applet_SOURCES =					\
	inhibit-applet.c					\
	egg-debug.c						\
	egg-debug.h						\
	gpm-common.c						\
	gpm-common.h

ukui_inhibit_applet_LDADD =					\
	$(DBUS_LIBS)						\
	$(CAIRO_LIBS)						\
	$(PANEL_LIBS)

appletdir       = $(datadir)/ukui-panel/applets
applet_in_files = org.ukui.InhibitApplet.ukui-panel-applet.in
applet_DATA     = $(applet_in_files:.ukui-panel-applet.in=.ukui-panel-applet)

$(applet_in_files): $(applet_in_files).in Makefile
	$(AM_V_GEN)sed \
            -e "s|\@LIBEXECDIR\@|$(libexecdir)|" \
            -e "s|\@VERSION\@|$(PACKAGE_VERSION)|" \
            $< > $@

%.ukui-panel-applet: %.ukui-panel-applet.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

servicedir       = $(datadir)/dbus-1/services
service_in_files = org.ukui.panel.applet.InhibitAppletFactory.service.in
service_DATA     = $(service_in_files:.service.in=.service)

org.ukui.panel.applet.InhibitAppletFactory.service: $(service_in_files)
	$(AM_V_GEN)sed \
            -e "s|\@LIBEXECDIR\@|$(libexecdir)|" \
            $< > $@

uidir   = $(datadir)/ukui-power-manager/ui
ui_DATA = inhibit-applet-menu.xml

EXTRA_DIST = org.ukui.InhibitApplet.ukui-panel-applet.in.in $(ui_DATA) $(service_in_files)

DISTCLEANFILES =						\
	$(applet_DATA) $(applet_DATA).in $(service_DATA)
