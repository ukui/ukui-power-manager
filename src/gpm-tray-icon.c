/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2005 William Jon McCann <mccann@jhu.edu>
 * Copyright (C) 2005-2009 Richard Hughes <richard@hughsie.com>
 * Copyright (C) 2013-2017 Xiang Li <lixiang@kylinos.cn>
 *
 * Licensed under the GNU General Public License Version 2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <libupower-glib/upower.h>

#include "egg-debug.h"

#include "gpm-upower.h"
#include "gpm-engine.h"
#include "gpm-common.h"
#include "gpm-icon-names.h"
#include "gpm-tray-icon.h"

//kobe
static gboolean has_cursor = FALSE;
static gboolean button_pressed = FALSE;

static void     gpm_tray_icon_finalize   (GObject	   *object);

#define GPM_TRAY_ICON_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GPM_TYPE_TRAY_ICON, GpmTrayIconPrivate))

struct GpmTrayIconPrivate
{
	GSettings		*settings;
	GpmEngine		*engine;
	GtkStatusIcon		*status_icon;
	gboolean		 show_actions;
        GtkWidget               *main_window;//kobe
        //GSettings               *panel_settings;//kobe
};

G_DEFINE_TYPE (GpmTrayIcon, gpm_tray_icon, G_TYPE_OBJECT)

/**
 * gpm_tray_icon_enable_actions:
 **/
static void
gpm_tray_icon_enable_actions (GpmTrayIcon *icon, gboolean enabled)
{
	g_return_if_fail (GPM_IS_TRAY_ICON (icon));
	icon->priv->show_actions = enabled;
}

/**
 * gpm_tray_icon_show:
 * @enabled: If we should show the tray
 **/
static void
gpm_tray_icon_show (GpmTrayIcon *icon, gboolean enabled)
{
	g_return_if_fail (GPM_IS_TRAY_ICON (icon));
	gtk_status_icon_set_visible (icon->priv->status_icon, enabled);
}

/**
 * gpm_tray_icon_set_tooltip:
 * @tooltip: The tooltip text, e.g. "Batteries charged"
 **/
gboolean
gpm_tray_icon_set_tooltip (GpmTrayIcon *icon, const gchar *tooltip)
{
	g_return_val_if_fail (icon != NULL, FALSE);
	g_return_val_if_fail (GPM_IS_TRAY_ICON (icon), FALSE);
	g_return_val_if_fail (tooltip != NULL, FALSE);

	gtk_status_icon_set_tooltip_text (icon->priv->status_icon, tooltip);

	return TRUE;
}

/**
 * gpm_tray_icon_get_status_icon:
 **/
GtkStatusIcon *
gpm_tray_icon_get_status_icon (GpmTrayIcon *icon)
{
	g_return_val_if_fail (GPM_IS_TRAY_ICON (icon), NULL);
	return g_object_ref (icon->priv->status_icon);
}

/**
 * gpm_tray_icon_set_icon:
 * @icon_name: The icon name, e.g. GPM_ICON_APP_ICON, or NULL to remove.
 *
 * Loads a pixmap from disk, and sets as the tooltip icon.
 **/
gboolean
gpm_tray_icon_set_icon (GpmTrayIcon *icon, const gchar *icon_name)
{
	g_return_val_if_fail (icon != NULL, FALSE);
	g_return_val_if_fail (GPM_IS_TRAY_ICON (icon), FALSE);

	if (icon_name != NULL) {
		egg_debug ("Setting icon to %s", icon_name);
		gtk_status_icon_set_from_icon_name (icon->priv->status_icon,
		                                    icon_name);

		/* make sure that we are visible */
		gpm_tray_icon_show (icon, TRUE);
	} else {
		/* remove icon */
		egg_debug ("no icon will be displayed");

		/* make sure that we are hidden */
		gpm_tray_icon_show (icon, FALSE);
	}
	return TRUE;
}

/**
 * gpm_tray_icon_show_info_cb:
 **/
static void
gpm_tray_icon_show_info_cb (GtkMenuItem *item, gpointer data)
{
	gchar *path;
	const gchar *object_path;

	object_path = g_object_get_data (G_OBJECT (item), "object-path");
	path = g_strdup_printf ("%s/ukui-power-statistics-qt --device %s", BINDIR, object_path);
	if (!g_spawn_command_line_async (path, NULL))
		egg_warning ("Couldn't execute command: %s", path);
	g_free (path);
}

//kobe
static gboolean
gpm_tray_icon_show_info_cb_ukui(GtkWidget *w, GdkEventButton *e, GpmTrayIcon *icon)
{
    gchar *path;
    const gchar *object_path;
    gtk_widget_hide (icon->priv->main_window);

    object_path = g_object_get_data (G_OBJECT (w), "object-path");
    path = g_strdup_printf ("%s/ukui-power-statistics-qt --device %s", BINDIR, object_path);
    if (!g_spawn_command_line_async (path, NULL))
        egg_warning ("Couldn't execute command: %s", path);
    g_free (path);

    return FALSE;
}

/**
 * gpm_tray_icon_show_preferences_cb:
 * @action: A valid GtkAction
 **/
static void
gpm_tray_icon_show_preferences_cb (GtkMenuItem *item, gpointer data)
{

    const gchar *command = "ukui-control-center -p &";

	if (g_spawn_command_line_async (command, NULL) == FALSE)
		egg_warning ("Couldn't execute command: %s", command);
}

//jiangh powerpolicy
static gboolean
gpm_tray_icon_power_policy (gint mode) {
    GError *error = NULL;
    GDBusProxy *proxy;
    GVariant *res = NULL;

    egg_debug ("Requesting power policy");
    proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
                           G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES,
                           NULL,
                           "ukui.power.policy",
                           "/ukui/power/policy",
                           "ukui.power.policy",
                           NULL,
                           &error );
    //append all our arguments
    if (proxy == NULL) {
        egg_debug("Error connecting to dbus - %s", error->message);
        g_error_free (error);
        return FALSE;
    }

    res = g_dbus_proxy_call_sync (proxy, "control",
                      g_variant_new( "(i)", mode),
                      G_DBUS_CALL_FLAGS_NONE,
                      -1,
                      NULL,
                      &error
                      );
    if (error != NULL) {
        egg_debug ("Error in dbus - %s", error->message);
        g_error_free (error);
        return FALSE;
    }

    g_variant_unref(res);
    return TRUE;
}

static void
gpm_tray_icon_show_perf_cb (GtkMenuItem *item, gpointer data)
{
    gpm_tray_icon_power_policy(0);
}

static void
gpm_tray_icon_show_save_cb (GtkMenuItem *item, gpointer data)
{
    gpm_tray_icon_power_policy(2);
}

/**
 * gpm_tray_icon_show_about_cb:
 * @action: A valid GtkAction
 **/
static void
gpm_tray_icon_show_about_cb (GtkMenuItem *item, gpointer data)
{
	const gchar *authors[] =
	{
		"Perberos",
		"Steve Zesch",
		"Stefano Karapetsas",
		NULL
	};

	gtk_show_about_dialog (NULL,
				"program-name", _("Power Manager"),
				"version", VERSION,
				"comments", _("Power management daemon"),
				"copyright", _("Copyright \xC2\xA9 2011-2017 UKUI developers"),
				"authors", authors,
				/* Translators should localize the following string
				* which will be displayed at the bottom of the about
				* box to give credit to the translator(s).
				*/
				"translator-credits", _("translator-credits"),
				"logo-icon-name", "ukui-power-manager",
				"website", "http://www.ukui.org",
				NULL);
}

/**
 * gpm_tray_icon_class_init:
 **/
static void
gpm_tray_icon_class_init (GpmTrayIconClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = gpm_tray_icon_finalize;
	g_type_class_add_private (klass, sizeof (GpmTrayIconPrivate));
}

/**
 * gpm_tray_icon_add_device:
 **/
static guint
gpm_tray_icon_add_device (GpmTrayIcon *icon, GtkMenu *menu, const GPtrArray *array, UpDeviceKind kind)
{
	guint i;
	guint added = 0;
	gchar *icon_name;
	gchar *label, *vendor, *model;
	GtkWidget *item;
	GtkWidget *image;
	const gchar *object_path;
	UpDevice *device;
	UpDeviceKind kind_tmp;
	gdouble percentage;

	/* find type */
	for (i=0;i<array->len;i++) {
		device = g_ptr_array_index (array, i);

		/* get device properties */
		g_object_get (device,
			      "kind", &kind_tmp,
			      "percentage", &percentage,
			      "vendor", &vendor,
			      "model", &model,
			      NULL);

		if (kind != kind_tmp)
			continue;

		object_path = up_device_get_object_path (device);
		egg_debug ("adding device %s", object_path);
		added++;

		/* generate the label */
		if ((vendor != NULL && strlen(vendor) != 0) && (model != NULL && strlen(model) != 0)) {
			label = g_strdup_printf ("%s %s (%.1f%%)", vendor, model, percentage);
		}
		else {
			label = g_strdup_printf ("%s (%.1f%%)", gpm_device_kind_to_localised_string (kind, 1), percentage);
		}
		item = gtk_image_menu_item_new_with_label (label);

		/* generate the image */
		icon_name = gpm_upower_get_device_icon (device);
		image = gtk_image_new_from_icon_name (icon_name, GTK_ICON_SIZE_MENU);
		gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), image);
		gtk_image_menu_item_set_always_show_image (GTK_IMAGE_MENU_ITEM (item), TRUE);

		/* set callback and add the menu */
		g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (gpm_tray_icon_show_info_cb), icon);
		g_object_set_data (G_OBJECT (item), "object-path", (gpointer) object_path);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

		g_free (icon_name);
		g_free (label);
		g_free (vendor);
		g_free (model);
	}
	return added;
}

//kobe
static guint
gpm_tray_icon_add_device_ukui (GpmTrayIcon *icon, GtkWidget *vbox, const GPtrArray *array, UpDeviceKind kind)
{
    guint i;
    guint added = 0;
    gchar *icon_name;
    gchar *text;
    GtkWidget *item;
    GtkWidget *image;
    const gchar *object_path;
    const gchar *desc;
    UpDevice *device;
    UpDeviceKind kind_tmp;
    UpDeviceState state;
    gdouble percentage;

    /* find type */
    for (i=0;i<array->len;i++) {
        device = g_ptr_array_index (array, i);
        /* get device properties */
        g_object_get (device,
                      "kind", &kind_tmp,
                      "state", &state,
                      "percentage", &percentage,
                      NULL);

        if (kind != kind_tmp)
            continue;

        object_path = up_device_get_object_path (device);
        egg_debug ("adding device %s", object_path);
        //g_printf("####adding device %s\n", object_path);///org/freedesktop/UPower/devices/battery_BAT1
        added++;

        GtkWidget *event_box = gtk_event_box_new ();
        gtk_event_box_set_visible_window (GTK_EVENT_BOX (event_box), FALSE);
        GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_container_add (GTK_CONTAINER(event_box), hbox);

        gboolean is_charging = FALSE;
        /* generate the image */
        icon_name = gpm_upower_get_device_icon (device);
        if (strstr(icon_name, "charging"))
            is_charging = TRUE;
        image = gtk_image_new_from_icon_name (icon_name, GTK_ICON_SIZE_DND);

        /* generate the label */
        desc = gpm_device_kind_to_localised_string (kind, 1);
        if (is_charging)
            text = g_strdup_printf (_("%.1f%% available power\n(The power is connected and is charging)"), percentage);
        else
            text = g_strdup_printf (_("%.1f%% available power"), percentage);

        item = gtk_label_new (text);

        gtk_box_pack_start (GTK_BOX(hbox), image, FALSE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX(hbox), item, FALSE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX(vbox), event_box, FALSE, FALSE, 0);

        g_object_set_data (event_box, "object-path", (gpointer) object_path);
        g_signal_connect (event_box, "button-press-event", G_CALLBACK (gpm_tray_icon_show_info_cb_ukui), icon);

        g_free (icon_name);
        g_free (text);
    }
    return added;
}

//kobe
static
void on_menu_pos (GtkMenu *menu, gint *x, gint *y, gboolean *push_in, GtkStatusIcon *icon)
{
    GdkRectangle rect;
    GdkRectangle monitor_rect;
    GtkOrientation orientation;
    GdkScreen *screen;
    int monitor_num;
    int menu_x = 0;
    int menu_y = 0;
    GtkRequisition menu_req;
    const static int panel_height = 40;

    screen = gtk_status_icon_get_screen (GTK_STATUS_ICON (icon));
    if (gtk_status_icon_get_geometry (icon, &screen, &rect, &orientation) == FALSE)
        goto out;

    monitor_num = gdk_screen_get_monitor_at_point (screen, rect.x, rect.y);
    gdk_screen_get_monitor_geometry (screen, monitor_num, &monitor_rect);

    gtk_widget_size_request (menu, &menu_req);

    if (orientation == GTK_ORIENTATION_VERTICAL) {
    } else {
        if (rect.y + rect.height + menu_req.height <= monitor_rect.y + monitor_rect.height)
            menu_y = monitor_rect.y + panel_height;
        else
            menu_y = monitor_rect.y + monitor_rect.height - panel_height - menu_req.height;

        if (rect.x + menu_req.width <= monitor_rect.x + monitor_rect.width)
            menu_x = rect.x;
        else
            menu_x = monitor_rect.x + monitor_rect.width - menu_req.width;
    }

out:
    *x = menu_x;
    *y = menu_y;
    *push_in = TRUE;
}

/**
 * gpm_tray_icon_add_primary_device:
 **/
static void
gpm_tray_icon_add_primary_device (GpmTrayIcon *icon, GtkMenu *menu, UpDevice *device)
{
	GtkWidget *item;
	gchar *time_str;
	gchar *string;
	gint64 time_to_empty = 0;

	/* get details */
	g_object_get (device,
		      "time-to-empty", &time_to_empty,
		      NULL);

	/* convert time to string */
	time_str = gpm_get_timestring (time_to_empty);

	/* TRANSLATORS: % is a timestring, e.g. "6 hours 10 minutes" */
	string = g_strdup_printf (_("%s remaining"), time_str);
	item = gtk_image_menu_item_new_with_label (string);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
	g_free (time_str);
	g_free (string);
}

static char* gpm_tray_caculate_theme (GtkWidget *widget)
{
    GdkColor color;
    GtkStyle *style=gtk_rc_get_style(widget);
    gtk_style_lookup_color (style,"panel_normal_bg_color",&color);

    char color_hex[10]={0};
    char color_hex_red[4]={0};
    char color_hex_green[4]={0};
    char color_hex_blue[4]={0};

    if((color.red/257/16)==0){
        sprintf(color_hex_red,"0%x",color.red/257);
    } else {
        sprintf(color_hex_red,"%x",color.red/257);
    }
    if((color.green/257)/16==0){
        sprintf(color_hex_green,"0%x",color.green/257);
    } else {
        sprintf(color_hex_green,"%x",color.green/257);
    }
    if((color.blue/257)/16==0){
        sprintf(color_hex_blue,"0%x",color.blue/257);
    } else {
        sprintf(color_hex_blue,"%x",color.blue/257);
    }
    sprintf(color_hex,"\#%s%s%s",color_hex_red,color_hex_green,color_hex_blue);
//    g_message("colorhex:%s----------------",color_hex);
    char *script =             "* {"
                               "color: #f2f2f2;"
                               "background-color: #283138;"
                               "opacity: 0.8;"
                               "}";
    if (!strcmp (color_hex, "#283138")){
        script =
            "* {"
            "color: #f2f2f2;"
            "background-color: #283138;"
            "opacity: 0.7;"
            "}"
            ;

    }
    else if(!strcmp (color_hex, "#f2f2f2"))
    {
        script =
            "* {"
            "color: #283138;"
            "background-color: #f2f2f2;"
            "opacity: 0.7;"
            "}"
            ;
    }
    return  script;
}

static gpm_tray_set_theme(GtkWidget *widget, const gchar *data)
{
    GtkCssProvider *provider = gtk_css_provider_new();
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_css_provider_load_from_data(provider,data,-1,NULL);
    gtk_style_context_add_provider(context,GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);
}

/**
 * gpm_tray_icon_create_menu:
 *
 * Create the popup menu.
 **/
static GtkMenu *
gpm_tray_icon_create_menu (GpmTrayIcon *icon)
{
	GtkMenu *menu = (GtkMenu*) gtk_menu_new ();
	GtkWidget *item;
	GtkWidget *image;
	guint dev_cnt = 0;
	GPtrArray *array;
	UpDevice *device = NULL;

	/* show the primary device time remaining */
	device = gpm_engine_get_primary_device (icon->priv->engine);
	if (device != NULL) {
		gpm_tray_icon_add_primary_device (icon, menu, device);
		item = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
	}

	/* add all device types to the drop down menu */
	array = gpm_engine_get_devices (icon->priv->engine);
	dev_cnt += gpm_tray_icon_add_device (icon, menu, array, UP_DEVICE_KIND_BATTERY);
	dev_cnt += gpm_tray_icon_add_device (icon, menu, array, UP_DEVICE_KIND_UPS);
	dev_cnt += gpm_tray_icon_add_device (icon, menu, array, UP_DEVICE_KIND_MOUSE);
	dev_cnt += gpm_tray_icon_add_device (icon, menu, array, UP_DEVICE_KIND_KEYBOARD);
	dev_cnt += gpm_tray_icon_add_device (icon, menu, array, UP_DEVICE_KIND_PDA);
	dev_cnt += gpm_tray_icon_add_device (icon, menu, array, UP_DEVICE_KIND_PHONE);
	dev_cnt += gpm_tray_icon_add_device (icon, menu, array, UP_DEVICE_KIND_MEDIA_PLAYER);
	dev_cnt += gpm_tray_icon_add_device (icon, menu, array, UP_DEVICE_KIND_TABLET);
	dev_cnt += gpm_tray_icon_add_device (icon, menu, array, UP_DEVICE_KIND_COMPUTER);
	g_ptr_array_unref (array);

	/* skip for things like live-cd's and GDM */
	if (!icon->priv->show_actions)
		goto skip_prefs;

	/* only do the separator if we have at least one device */
	if (dev_cnt != 0) {
		item = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
	}

	/* preferences */
	item = gtk_image_menu_item_new_with_mnemonic (_("_Preferences"));
	image = gtk_image_new_from_icon_name ("preferences-system", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), image);
	g_signal_connect (G_OBJECT (item), "activate",
			  G_CALLBACK (gpm_tray_icon_show_preferences_cb), icon);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

    /*powerpolicy*/
    /*
    item = gtk_image_menu_item_new_with_mnemonic (_("_Performance Mode"));
    g_signal_connect (G_OBJECT (item), "activate",
              G_CALLBACK (gpm_tray_icon_show_perf_cb), icon);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
    item = gtk_image_menu_item_new_with_mnemonic (_("_Save Mode"));
    g_signal_connect (G_OBJECT (item), "activate",
              G_CALLBACK (gpm_tray_icon_show_save_cb), icon);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
    */
	
	/*Set up custom panel menu theme support-gtk3 only */
	GtkWidget *toplevel = gtk_widget_get_toplevel (GTK_WIDGET (menu));
	/* Fix any failures of compiz/other wm's to communicate with gtk for transparency in menu theme */
	GdkScreen *screen = gtk_widget_get_screen(GTK_WIDGET(toplevel));
	GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
	gtk_widget_set_visual(GTK_WIDGET(toplevel), visual);
	/* Set menu and its toplevel window to follow panel theme */
	GtkStyleContext *context;
	context = gtk_widget_get_style_context (GTK_WIDGET(toplevel));
	gtk_style_context_add_class(context,"gnome-panel-menu-bar");
	gtk_style_context_add_class(context,"ukui-panel-menu-bar");

	/* about */
	item = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT, NULL);
	g_signal_connect (G_OBJECT (item), "activate",
			  G_CALLBACK (gpm_tray_icon_show_about_cb), icon);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

    gchar *css = gpm_tray_caculate_theme(GTK_WIDGET (menu));
   	gpm_tray_set_theme(GTK_WIDGET(menu),css);
skip_prefs:
	if (device != NULL)
		g_object_unref (device);
	return menu;
}

/**
 * gpm_tray_icon_popup_cleared_cd:
 * @widget: The popup Gtkwidget
 *
 * We have to re-enable the tooltip when the popup is removed
 **/
static void
gpm_tray_icon_popup_cleared_cd (GtkWidget *widget, GpmTrayIcon *icon)
{
	g_return_if_fail (GPM_IS_TRAY_ICON (icon));
	egg_debug ("clear tray");
	g_object_ref_sink (widget);
	g_object_unref (widget);
}

/**
 * gpm_tray_icon_popup_menu:
 *
 * Display the popup menu.
 **/
static void
gpm_tray_icon_popup_menu (GpmTrayIcon *icon, guint32 timestamp)
{
	GtkMenu *menu;

	menu = gpm_tray_icon_create_menu (icon);

	/* show the menu */
	gtk_widget_show_all (GTK_WIDGET (menu));
	gtk_menu_popup (GTK_MENU (menu), NULL, NULL,
			gtk_status_icon_position_menu, icon->priv->status_icon,
			1, timestamp);

	g_signal_connect (GTK_WIDGET (menu), "hide",
			  G_CALLBACK (gpm_tray_icon_popup_cleared_cd), icon);
}

/**
 * gpm_tray_icon_popup_menu_cb:
 *
 * Display the popup menu.
 **/
static void
gpm_tray_icon_popup_menu_cb (GtkStatusIcon *status_icon, guint button, guint32 timestamp, GpmTrayIcon *icon)
{
	egg_debug ("icon right clicked");

        gpm_tray_icon_popup_menu (icon, timestamp);
        //kobe
        /*if (icon->priv->main_window == NULL || GTK_IS_WINDOW(icon->priv->main_window) == FALSE)
            return;
        if (gtk_widget_get_visible (icon->priv->main_window))
            gtk_widget_hide (icon->priv->main_window);*/
}

//kobe
static void
gpm_tray_icon_add_vbox_content (GpmTrayIcon *icon, GtkWidget *vbox)
{
    //GtkWidget *item;
    //GtkWidget *image;
    guint dev_cnt = 0;
    GPtrArray *array;

    /* add all device types to the drop down menu */
    array = gpm_engine_get_devices (icon->priv->engine);
    dev_cnt += gpm_tray_icon_add_device_ukui (icon, vbox, array, UP_DEVICE_KIND_BATTERY);
#if UP_CHECK_VERSION(0,9,5)
    dev_cnt += gpm_tray_icon_add_device_ukui (icon, vbox, array, UP_DEVICE_KIND_COMPUTER);
#endif
    g_ptr_array_unref (array);

    /* skip for things like live-cd's and GDM */
    if (!icon->priv->show_actions)
        return;

    /* only do the seporator if we have at least one device */
    if (dev_cnt != 0) {
//        item = gtk_separator_menu_item_new ();
//        gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
    } else {
        gtk_container_set_border_width (GTK_CONTAINER (vbox), 0);
    }
}

//kobe
//static gboolean draw_pref_button (GtkWidget *widget,  GdkEventExpose *e, gpointer user_data)
void draw_pref_button (GtkWidget *widget, cairo_t *cr, cairo_surface_t *img)
{
    //cairo_t *cr = gdk_cairo_create(widget->window);

    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);

    static double r_bg = 1.0, g_bg = 1.0, b_bg = 1.0, a_bg = 1.0;

    if (has_cursor == FALSE) {
        // normal
        r_bg = g_bg = b_bg = 0.9;
    } else if (button_pressed == FALSE) {
        // hover
        r_bg = g_bg = b_bg = 0.9;
    } else {
        // press
        r_bg = g_bg = b_bg = 0.87;
    }

    cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
    cairo_set_line_width(cr, 1.0);
    cairo_move_to(cr, alloc.x + 0.5, alloc.y + 0.5);
    cairo_line_to(cr, alloc.x + alloc.width, alloc.y + 0.5);
    cairo_stroke(cr);

    cairo_set_source_rgba(cr, r_bg, g_bg, b_bg, a_bg);
    cairo_rectangle(cr, alloc.x + 0.5, alloc.y + 1.5, alloc.width - 0.5, alloc.height - 1.5);
    cairo_fill(cr);

    PangoFontDescription *font_desc = pango_font_description_new();
    pango_font_description_set_size(font_desc, 11 * PANGO_SCALE);
    pango_font_description_set_family(font_desc, "Ubuntu");

    PangoLayout *layout = pango_cairo_create_layout(cr);
    pango_layout_set_font_description(layout, font_desc);
    pango_layout_set_text(layout, _("Power Preferences"), -1);

    int width, height;
    static const int padding = 3;
    pango_layout_get_pixel_size(layout, &width, &height);
    if (alloc.height < height + 2 * padding)
        gtk_widget_set_size_request(widget, -1, height + 2 * padding);
    if (alloc.width < width + 2 * padding)
        gtk_widget_set_size_request(widget, width + 2 * padding, -1);

    cairo_move_to(cr, alloc.x + (alloc.width - width) / 2, alloc.y + (alloc.height - height) / 2);
    cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
    pango_cairo_show_layout(cr, layout);

    g_object_unref (layout);
    pango_font_description_free (font_desc);
    cairo_destroy(cr);

    //return TRUE;
}

//kobe
static gboolean
on_button_press(GtkWidget *button, GdkEventButton *event, gpointer user_data)
{
    if (event->button == 1) {
        button_pressed = TRUE;
        gtk_widget_queue_draw(button);
    }

    return FALSE;
}

//kobe
static gboolean
on_button_release(GtkWidget *button, GdkEventButton *event, gpointer user_data)
{
    if (event->button == 1) {
        button_pressed = FALSE;
        gtk_widget_queue_draw(button);
    }

    return FALSE;
}

//kobe
static gboolean
on_mouse_enter(GtkWidget *w, GdkEventCrossing *event, gpointer user_data)
{
    has_cursor = TRUE;
    gtk_widget_queue_draw(w);

    return FALSE;
}

//kobe
static gboolean
on_mouse_leave(GtkWidget *w, GdkEventCrossing *event, gpointer user_data)
{
    has_cursor = FALSE;
    gtk_widget_queue_draw(w);

    return FALSE;
}

//kobe
GdkColor get_border_color (GtkWidget *w, GpmTrayIcon *icon, char *color_name)
{
    GdkColor color;

    GtkSettings *gs = gtk_settings_get_for_screen (gtk_status_icon_get_screen (icon->priv->status_icon));
    GValue color_scheme_value = G_VALUE_INIT;
    g_value_init (&color_scheme_value, G_TYPE_STRING);
    g_object_get_property (gs, "gtk-color-scheme", &color_scheme_value);
    const gchar *color_scheme = g_value_get_string (&color_scheme_value);
    gchar color_spec[16] = { 0 };
    //char *needle = strstr(color_scheme, "kylinmenu_color");
    char *needle = strstr(color_scheme, color_name);
    if (needle) {
        while (1) {
            if (color_spec[0] != '#') {
                color_spec[0] = *needle;
                needle++;
                continue;
            }

            if ((*needle >= 0x30 && *needle <= 0x39) ||
                (*needle >= 0x41 && *needle <= 0x46) ||
                (*needle >= 0x61 && *needle <= 0x66)) {
                color_spec[strlen(color_spec)] = *needle;
                needle++;
            } else {
                break;
            }
        }
        gdk_color_parse (color_spec, &color);
    } else {
        gdk_color_parse ("#3B9DC5", &color);
    }

    //gtk_widget_modify_bg (w, GTK_STATE_NORMAL, &color);
    return color;
}

//kobe
//static gboolean draw_border(GtkWidget *w, GdkEventExpose *e, gpointer user_data)
void draw_border (GtkWidget *w, cairo_t *cr, cairo_surface_t *user_data)
{
    GpmTrayIcon *icon = (GpmTrayIcon *)user_data;
    //cairo_t *cr = gdk_cairo_create (w->window);

    GtkAllocation alloc;
    gtk_widget_get_allocation (w, &alloc);

    GdkColor color;
    color = get_border_color(w, icon, "taskbar_applet_border_color");//kylinside_color
    // outer border
    cairo_set_source_rgb (cr, color.red / 65535.0, color.green / 65535.0, color.blue / 65535.0);
    cairo_set_line_width (cr, 1.0);
    cairo_rectangle (cr, 0.5, 0.5, alloc.width - 1, alloc.height - 1);
    cairo_stroke (cr);
    cairo_destroy (cr);

    //return FALSE;
}

/**
 * gpm_tray_icon_activate_cb:
 * @button: Which buttons are pressed
 *
 * Callback when the icon is clicked
 **/
static void
gpm_tray_icon_activate_cb (GtkStatusIcon *status_icon, GpmTrayIcon *icon)
{
	egg_debug ("icon left clicked");
        gpm_tray_icon_popup_menu (icon, gtk_get_current_event_time());

        //kobe
        /*GdkColor color;
        if (icon->priv->main_window) {
            if (gtk_widget_get_visible (icon->priv->main_window)) {
                gtk_widget_destroy (icon->priv->main_window);
                icon->priv->main_window = NULL;
                return;
            } else {
                gtk_widget_destroy (icon->priv->main_window);
                icon->priv->main_window = NULL;
            }
        }

        GtkWidget *pref_button = gtk_button_new ();
        gtk_widget_set_size_request (pref_button, 150, 32);
        g_signal_connect (G_OBJECT(pref_button),"draw", G_CALLBACK (draw_pref_button), NULL);
        //g_signal_connect (pref_button, "expose-event", G_CALLBACK (draw_pref_button), NULL);
        g_signal_connect (pref_button, "button-press-event", G_CALLBACK(on_button_press), NULL);
        g_signal_connect (pref_button, "button-release-event", G_CALLBACK(on_button_release), NULL);
        g_signal_connect (pref_button, "enter-notify-event", G_CALLBACK(on_mouse_enter), NULL);
        g_signal_connect (pref_button, "leave-notify-event", G_CALLBACK(on_mouse_leave), NULL);

        GtkWidget *vbox = gtk_vbox_new (FALSE, 6);
        gtk_container_set_border_width (GTK_CONTAINER (vbox), 10);
        gpm_tray_icon_add_vbox_content (icon, vbox);

        GtkWidget *vbox2 = gtk_vbox_new (FALSE, 0);
        gtk_box_pack_start (GTK_BOX(vbox2), vbox, FALSE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX(vbox2), pref_button, TRUE, TRUE, 0);

        GtkWidget *event_box = gtk_event_box_new ();
        gtk_container_add (GTK_CONTAINER (event_box), vbox2);
        gdk_color_parse ("#fefefe", &color);
        gtk_widget_modify_bg (event_box, GTK_STATE_NORMAL, &color);

        icon->priv->main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
        gtk_window_set_screen (GTK_WINDOW(icon->priv->main_window), gtk_status_icon_get_screen (icon->priv->status_icon));
        gtk_window_set_decorated(GTK_WINDOW(icon->priv->main_window), FALSE);
        gtk_window_set_skip_pager_hint(GTK_WINDOW(icon->priv->main_window), TRUE);
        gtk_window_set_skip_taskbar_hint(GTK_WINDOW(icon->priv->main_window), TRUE);
        gtk_window_set_position(GTK_WINDOW(icon->priv->main_window), GTK_WIN_POS_MOUSE);
        gtk_window_set_gravity(GTK_WINDOW(icon->priv->main_window), GDK_GRAVITY_SOUTH_EAST);

        gtk_widget_hide_on_delete (icon->priv->main_window);
        gtk_container_set_border_width (GTK_CONTAINER (icon->priv->main_window), 1);
        gtk_container_add (GTK_CONTAINER (icon->priv->main_window), event_box);
        g_signal_connect (icon->priv->main_window, "focus-out-event", G_CALLBACK(gtk_widget_hide), NULL);
        //g_signal_connect_after (icon->priv->main_window, "expose-event", G_CALLBACK(draw_border), icon);
        g_signal_connect_after (icon->priv->main_window, "draw", G_CALLBACK(draw_border), icon);

        GdkColor color2 = get_border_color(icon->priv->main_window, icon, "taskbar_applet_border_color");//kylinmenu_color
        gtk_widget_modify_bg (icon->priv->main_window, GTK_STATE_NORMAL, &color2);

        g_signal_connect (pref_button, "clicked", G_CALLBACK (gpm_tray_icon_show_preferences_cb), NULL);
        g_signal_connect_swapped (pref_button, "clicked", G_CALLBACK (gtk_widget_hide), icon->priv->main_window);

        gtk_widget_show_all (icon->priv->main_window);*/
}

/**
 * gpm_tray_icon_settings_changed_cb:
 *
 * We might have to do things when the settings change; do them here.
 **/
static void
gpm_tray_icon_settings_changed_cb (GSettings *settings, const gchar *key, GpmTrayIcon *icon)
{
	gboolean allowed_in_menu;

	if (g_strcmp0 (key, GPM_SETTINGS_SHOW_ACTIONS) == 0) {
		allowed_in_menu = g_settings_get_boolean (settings, key);
		gpm_tray_icon_enable_actions (icon, allowed_in_menu);
	}
}

/**
 * gpm_tray_icon_init:
 *
 * Initialise the tray object
 **/
static void
gpm_tray_icon_init (GpmTrayIcon *icon)
{
	gboolean allowed_in_menu;

	icon->priv = GPM_TRAY_ICON_GET_PRIVATE (icon);

	icon->priv->engine = gpm_engine_new ();

        //kobe
        //icon->priv->panel_settings = g_settings_new_with_path ("org.ukui.panel.toplevel", "/org/ukui/panel/toplevels/bottom/");

	icon->priv->settings = g_settings_new (GPM_SETTINGS_SCHEMA);
	g_signal_connect (icon->priv->settings, "changed",
			  G_CALLBACK (gpm_tray_icon_settings_changed_cb), icon);

	icon->priv->status_icon = gtk_status_icon_new ();
	gpm_tray_icon_show (icon, FALSE);
	g_signal_connect_object (G_OBJECT (icon->priv->status_icon),
				 "popup_menu",
				 G_CALLBACK (gpm_tray_icon_popup_menu_cb),
				 icon, 0);
	g_signal_connect_object (G_OBJECT (icon->priv->status_icon),
				 "activate",
				 G_CALLBACK (gpm_tray_icon_activate_cb),
				 icon, 0);

	allowed_in_menu = g_settings_get_boolean (icon->priv->settings, GPM_SETTINGS_SHOW_ACTIONS);
	gpm_tray_icon_enable_actions (icon, allowed_in_menu);
}

/**
 * gpm_tray_icon_finalize:
 * @object: This TrayIcon class instance
 **/
static void
gpm_tray_icon_finalize (GObject *object)
{
	GpmTrayIcon *tray_icon;

	g_return_if_fail (object != NULL);
	g_return_if_fail (GPM_IS_TRAY_ICON (object));

	tray_icon = GPM_TRAY_ICON (object);

        //kobe
        //g_object_unref (tray_icon->priv->panel_settings);

	g_object_unref (tray_icon->priv->status_icon);
	g_object_unref (tray_icon->priv->engine);
	g_return_if_fail (tray_icon->priv != NULL);

	G_OBJECT_CLASS (gpm_tray_icon_parent_class)->finalize (object);
}

/**
 * gpm_tray_icon_new:
 * Return value: A new TrayIcon object.
 **/
GpmTrayIcon *
gpm_tray_icon_new (void)
{
	GpmTrayIcon *tray_icon;
	tray_icon = g_object_new (GPM_TYPE_TRAY_ICON, NULL);
	return GPM_TRAY_ICON (tray_icon);
}

