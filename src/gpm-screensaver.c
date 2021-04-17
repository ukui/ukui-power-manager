/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2006-2007 Richard Hughes <richard@hughsie.com>
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

#include "config.h"

#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
//#include <dbus/dbus-glib.h>


#include "gpm-screensaver.h"
#include "gpm-common.h"
#include "egg-debug.h"
#include <gio/gio.h>

static void     gpm_screensaver_finalize   (GObject		*object);

#define GPM_SCREENSAVER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GPM_TYPE_SCREENSAVER, GpmScreensaverPrivate))

#define GS_LISTENER_SERVICE	"org.ukui.ScreenSaver"
#define GS_LISTENER_PATH	"/"
#define GS_LISTENER_INTERFACE	"org.ukui.ScreenSaver"

struct GpmScreensaverPrivate
{
	GDBusProxy		*proxy;
};


static gpointer gpm_screensaver_object = NULL;

G_DEFINE_TYPE (GpmScreensaver, gpm_screensaver, G_TYPE_OBJECT)

/**
 * gpm_screensaver_lock
 * @screensaver: This class instance
 * Return value: Success value
 **/
gboolean
gpm_screensaver_lock (GpmScreensaver *screensaver)
{
	guint sleepcount = 0;

	g_return_val_if_fail (GPM_IS_SCREENSAVER (screensaver), FALSE);

	if (screensaver->priv->proxy == NULL) {
		egg_warning ("not connected");
		return FALSE;
	}

//	egg_debug ("doing mate-screensaver lock");
	/*dbus_g_proxy_call_no_reply (screensaver->priv->proxy,
				    "Lock", G_TYPE_INVALID);*/
        g_dbus_proxy_call (screensaver->priv->proxy,
                           "Lock",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE, -1,
                           NULL, NULL, NULL);


	/* When we send the Lock signal to g-ss it takes maybe a second
	   or so to fade the screen and lock. If we suspend mid fade then on
	   resume the X display is still present for a split second
	   (since fade is gamma) and as such it can leak information.
	   Instead we wait until g-ss reports running and thus blanked
	   solidly before we continue from the screensaver_lock action.
	   The interior of g-ss is async, so we cannot get the dbus method
	   to block until lock is complete. */
	while (! gpm_screensaver_check_running (screensaver)) {
		/* Sleep for 1/10s */
		g_usleep (1000 * 100);
		if (sleepcount++ > 30) {
//			egg_debug ("timeout waiting for ukui-screensaver");
			break;
		}
	}

	return TRUE;
}

/**
 * gpm_screensaver_add_throttle:
 * @screensaver: This class instance
 * @reason:      The reason for throttling
 * Return value: Success value, or zero for failure
 **/
guint
gpm_screensaver_add_throttle (GpmScreensaver *screensaver,
			      const char     *reason)
{
	GError  *error = NULL;
	gboolean ret;
	guint32  cookie;
	
	GVariant *u_cookie;
	g_return_val_if_fail (GPM_IS_SCREENSAVER (screensaver), 0);
	g_return_val_if_fail (reason != NULL, 0);

	if (screensaver->priv->proxy == NULL) {
		egg_warning ("not connected");
		return 0;
	}

	/*ret = dbus_g_proxy_call (screensaver->priv->proxy,
				 "Throttle", &error,
				 G_TYPE_STRING, "Power screensaver",
				 G_TYPE_STRING, reason,
				 G_TYPE_INVALID,
				 G_TYPE_UINT, &cookie,
				 G_TYPE_INVALID);
	if (error) {
		egg_debug ("ERROR: %s", error->message);
		g_error_free (error);
	}
	if (!ret) {
		egg_warning ("Throttle failed!");
		return 0;
	}
	egg_debug ("adding throttle reason: '%s': id %u", reason, cookie);*/

        u_cookie = g_dbus_proxy_call_sync (screensaver->priv->proxy,
                           "Throttle",
                           g_variant_new ("(ss)","Power screensaver", reason),
                           G_DBUS_CALL_FLAGS_NONE, -1,
                           NULL, &error);

        if (u_cookie == NULL)
        {
                g_error_free (error);
                return FALSE;
        }
	g_variant_get (u_cookie, "(u)", &cookie);
	g_variant_unref (u_cookie);

	return cookie;
}

/**
 * gpm_screensaver_remove_throttle:
 **/
gboolean
gpm_screensaver_remove_throttle (GpmScreensaver *screensaver, guint cookie)
{
	gboolean ret;
	GError *error = NULL;
	GVariant *b_state;

	g_return_val_if_fail (GPM_IS_SCREENSAVER (screensaver), FALSE);

	if (screensaver->priv->proxy == NULL) {
		egg_warning ("not connected");
		return FALSE;
	}

	/*egg_debug ("removing throttle: id %u", cookie);
	ret = dbus_g_proxy_call (screensaver->priv->proxy,
				 "UnThrottle", &error,
				 G_TYPE_UINT, cookie,
				 G_TYPE_INVALID,
				 G_TYPE_INVALID);
	if (error) {
		egg_debug ("ERROR: %s", error->message);
		g_error_free (error);
	}
	if (!ret) {
		egg_warning ("UnThrottle failed!");
		return FALSE;
	}*/
        g_dbus_proxy_call_sync (screensaver->priv->proxy,
                           "UnThrottle",
                           g_variant_new ("(u)",(guint)cookie),
                           G_DBUS_CALL_FLAGS_NONE, -1,
                           NULL, &error);

        if (error != NULL)
        {
                g_error_free (error);
                return FALSE;
        }


	return TRUE;
}

/**
 * gpm_screensaver_check_running:
 * @screensaver: This class instance
 * Return value: TRUE if ukui-screensaver is running
 **/
gboolean
gpm_screensaver_check_running (GpmScreensaver *screensaver)
{
	gboolean ret;
	gboolean temp = TRUE;
	GError *error = NULL;

	GVariant *b_state;

	g_return_val_if_fail (GPM_IS_SCREENSAVER (screensaver), FALSE);

	if (screensaver->priv->proxy == NULL) {
		egg_warning ("not connected");
		return FALSE;
	}

	/*ret = dbus_g_proxy_call (screensaver->priv->proxy,
				 "GetActive", &error,
				 G_TYPE_INVALID,
				 G_TYPE_BOOLEAN, &temp,
				 G_TYPE_INVALID);*/
        /*ret = dbus_g_proxy_call (screensaver->priv->proxy,
                                 "GetLockState", &error,
                                 G_TYPE_INVALID,
                                 G_TYPE_BOOLEAN, &temp,
                                 G_TYPE_INVALID);*/
	

        b_state = g_dbus_proxy_call_sync (screensaver->priv->proxy,
                           "GetLockState",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE, -1,
                           NULL, &error);

        if (b_state == NULL)
        {
                g_error_free (error);
                return FALSE;
        }

	g_variant_get (b_state, "(b)", &ret);
	g_variant_unref (b_state);
	g_message ("GetLockState[%d]",ret);
	return ret;
}

/**
 * gpm_screensaver_poke:
 * @screensaver: This class instance
 *
 * Pokes UKUI Screensaver simulating hardware events. This displays the unlock
 * dialogue when we resume, so the user doesn't have to move the mouse or press
 * any key before the window comes up.
 **/
gboolean
gpm_screensaver_poke (GpmScreensaver *screensaver)
{
	GError *error = NULL;
	GVariant *ret = NULL;

	g_return_val_if_fail (GPM_IS_SCREENSAVER (screensaver), FALSE);

	if (screensaver->priv->proxy == NULL) {
		egg_warning ("not connected");
		return FALSE;
	}

	egg_debug ("poke");
	/*dbus_g_proxy_call_no_reply (screensaver->priv->proxy,
				    "SimulateUserActivity",
				    G_TYPE_INVALID);*/
	g_dbus_proxy_call (screensaver->priv->proxy,
			   "SimulateUserActivity",
			   NULL,
			   G_DBUS_CALL_FLAGS_NONE, -1,
    			   NULL, NULL, NULL);








	return TRUE;
}

/**
 * gpm_screensaver_class_init:
 * @klass: This class instance
 **/
static void
gpm_screensaver_class_init (GpmScreensaverClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = gpm_screensaver_finalize;
	g_type_class_add_private (klass, sizeof (GpmScreensaverPrivate));

}

/**
 * gpm_screensaver_init:
 * @screensaver: This class instance
 **/
static void
gpm_screensaver_init (GpmScreensaver *screensaver)
{
//	DBusGConnection *connection;

	GError *error = NULL;
	screensaver->priv = GPM_SCREENSAVER_GET_PRIVATE (screensaver);

	//connection = dbus_g_bus_get (DBUS_BUS_SESSION, NULL);
	/*connection = g_bus_get_sync (DBUS_BUS_SESSION, NULL, error);
	screensaver->priv->proxy = dbus_g_proxy_new_for_name (connection,
							      GS_LISTENER_SERVICE,
							      GS_LISTENER_PATH,
							      GS_LISTENER_INTERFACE);*/
        screensaver->priv->proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SESSION,
                                      G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES,
                                      NULL,
                                      GS_LISTENER_SERVICE,
                                      GS_LISTENER_PATH,
                                      GS_LISTENER_INTERFACE,
                                      NULL,
                                      &error);
	if (error != NULL)
		g_error_free (error);

}

/**
 * gpm_screensaver_finalize:
 * @object: This class instance
 **/
static void
gpm_screensaver_finalize (GObject *object)
{
	GpmScreensaver *screensaver;
	g_return_if_fail (object != NULL);
	g_return_if_fail (GPM_IS_SCREENSAVER (object));

	screensaver = GPM_SCREENSAVER (object);
	screensaver->priv = GPM_SCREENSAVER_GET_PRIVATE (screensaver);

	g_object_unref (screensaver->priv->proxy);

	G_OBJECT_CLASS (gpm_screensaver_parent_class)->finalize (object);
}

/**
 * gpm_screensaver_new:
 * Return value: new GpmScreensaver instance.
 **/
GpmScreensaver *
gpm_screensaver_new (void)
{
	if (gpm_screensaver_object != NULL) {
		g_object_ref (gpm_screensaver_object);
	} else {
		gpm_screensaver_object = g_object_new (GPM_TYPE_SCREENSAVER, NULL);
		g_object_add_weak_pointer (gpm_screensaver_object, &gpm_screensaver_object);
	}
	return GPM_SCREENSAVER (gpm_screensaver_object);
}
/***************************************************************************
 ***                          MAKE CHECK TESTS                           ***
 ***************************************************************************/
#ifdef EGG_TEST
#include "egg-test.h"

void
gpm_screensaver_test (gpointer data)
{
	GpmScreensaver *screensaver;
//	guint value;
	gboolean ret;
	EggTest *test = (EggTest *) data;

	if (egg_test_start (test, "GpmScreensaver") == FALSE)
		return;

	/************************************************************/
	egg_test_title (test, "make sure we get a non null screensaver");
	screensaver = gpm_screensaver_new ();
	egg_test_assert (test, (screensaver != NULL));

	/************************************************************/
	egg_test_title (test, "lock screensaver");
	ret = gpm_screensaver_lock (screensaver);
	egg_test_assert (test, ret);

	/************************************************************/
	egg_test_title (test, "poke screensaver");
	ret = gpm_screensaver_poke (screensaver);
	egg_test_assert (test, ret);

	g_object_unref (screensaver);

	egg_test_end (test);
}

#endif

