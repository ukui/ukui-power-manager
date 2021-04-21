/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2006-2008 Richard Hughes <richard@hughsie.com>
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
#include <unistd.h>
#include <stdio.h>
#include <glib.h>
#include <dbus/dbus.h>
#include <gio/gio.h>

#include "egg-debug.h"
#include "egg-console-kit.h"

static void     egg_console_kit_finalize	(GObject		*object);

#define EGG_CONSOLE_KIT_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), EGG_TYPE_CONSOLE_KIT, EggConsoleKitPrivate))

#define CONSOLEKIT_NAME			"org.freedesktop.ConsoleKit"
#define CONSOLEKIT_PATH			"/org/freedesktop/ConsoleKit"
#define CONSOLEKIT_INTERFACE		"org.freedesktop.ConsoleKit"

#define CONSOLEKIT_MANAGER_PATH	 	"/org/freedesktop/ConsoleKit/Manager"
#define CONSOLEKIT_MANAGER_INTERFACE    "org.freedesktop.ConsoleKit.Manager"
#define CONSOLEKIT_SEAT_INTERFACE       "org.freedesktop.ConsoleKit.Seat"
#define CONSOLEKIT_SESSION_INTERFACE    "org.freedesktop.ConsoleKit.Session"

struct EggConsoleKitPrivate
{
	//DBusGConnection		*connection;
	GDBusProxy		*proxy_manager;
	GDBusProxy		*proxy_session;
	gchar			*session_id;
};

enum {
	EGG_CONSOLE_KIT_ACTIVE_CHANGED,
	EGG_CONSOLE_KIT_LAST_SIGNAL
};

static gpointer egg_console_kit_object = NULL;
static guint signals [EGG_CONSOLE_KIT_LAST_SIGNAL] = { 0 };
G_DEFINE_TYPE (EggConsoleKit, egg_console_kit, G_TYPE_OBJECT)

/**
 * egg_console_kit_restart:
 **/
gboolean
egg_console_kit_restart (EggConsoleKit *console, GError **error)
{
	gboolean ret;
	GError *error_local = NULL;

	g_return_val_if_fail (EGG_IS_CONSOLE_KIT (console), FALSE);
	g_return_val_if_fail (console->priv->proxy_manager != NULL, FALSE);

         g_dbus_proxy_call_sync (console->priv->proxy_manager,
                           "Restart",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE, -1,
                           NULL, &error_local);

        if (error_local != NULL)
        {
                g_error_free (error_local);
                return FALSE;
        }

	return ret;
}

/**
 * egg_console_kit_stop:
 **/
gboolean
egg_console_kit_stop (EggConsoleKit *console, GError **error)
{
	gboolean ret;
	GError *error_local = NULL;

	g_return_val_if_fail (EGG_IS_CONSOLE_KIT (console), FALSE);
	g_return_val_if_fail (console->priv->proxy_manager != NULL, FALSE);

	GVariant * u_cookie;
         g_dbus_proxy_call_sync (console->priv->proxy_manager,
                           "Stop",
			   NULL,
                           G_DBUS_CALL_FLAGS_NONE, -1,
                           NULL, &error_local);

        if (error_local != NULL)
        {
                g_error_free (error_local);
                return FALSE;
        }
	return TRUE;
}

/**
 * egg_console_kit_suspend:
 **/
gboolean
egg_console_kit_suspend (EggConsoleKit *console, GError **error)
{
	gboolean ret;
	GError *error_local = NULL;

	g_return_val_if_fail (EGG_IS_CONSOLE_KIT (console), FALSE);
	g_return_val_if_fail (console->priv->proxy_manager != NULL, FALSE);

        g_dbus_proxy_call_sync (console->priv->proxy_manager,
                           "Suspend",
                           g_variant_new("(b)",TRUE),
                           G_DBUS_CALL_FLAGS_NONE, -1,
                           NULL, &error_local);

        if (error_local != NULL)
        {
                g_error_free (error_local);
                return FALSE;
        }
        return TRUE;
}

/**
 * egg_console_kit_hibernate:
 **/
gboolean
egg_console_kit_hibernate (EggConsoleKit *console, GError **error)
{
	gboolean ret;
	GError *error_local = NULL;

	g_return_val_if_fail (EGG_IS_CONSOLE_KIT (console), FALSE);
	g_return_val_if_fail (console->priv->proxy_manager != NULL, FALSE);

         g_dbus_proxy_call_sync (console->priv->proxy_manager,
                           "Hibernate",
			   NULL,
                           G_DBUS_CALL_FLAGS_NONE, -1,
                           NULL, &error_local);

        if (error_local != NULL)
        {
                g_error_free (error_local);
                return FALSE;
        }
	return TRUE;

}

/**
 * egg_console_kit_can_stop:
 **/
gboolean
egg_console_kit_can_stop (EggConsoleKit *console, gboolean *can_stop, GError **error)
{
	gboolean ret;
	GError *error_local = NULL;

	g_return_val_if_fail (EGG_IS_CONSOLE_KIT (console), FALSE);
	g_return_val_if_fail (console->priv->proxy_manager != NULL, FALSE);


        GVariant * u_cookie;
        u_cookie = g_dbus_proxy_call_sync (console->priv->proxy_manager,
                           "CanStop",
			   NULL,
                           G_DBUS_CALL_FLAGS_NONE, -1,
                           NULL, &error_local);

        if (u_cookie == NULL)
        {
                g_error_free (error_local);
                return FALSE;
        }
        g_variant_get (u_cookie, "(b)", can_stop);
        g_variant_unref (u_cookie);

	return TRUE;
}

/**
 * egg_console_kit_can_restart:
 **/
gboolean
egg_console_kit_can_restart (EggConsoleKit *console, gboolean *can_restart, GError **error)
{
	gboolean ret;
	GError *error_local = NULL;

	g_return_val_if_fail (EGG_IS_CONSOLE_KIT (console), FALSE);
	g_return_val_if_fail (console->priv->proxy_manager != NULL, FALSE);


        GVariant * u_cookie;
        u_cookie = g_dbus_proxy_call_sync (console->priv->proxy_manager,
                           "CanRestart",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE, -1,
                           NULL, &error_local);

        if (u_cookie == NULL)
        {
                g_error_free (error_local);
                return FALSE;
        }
        g_variant_get (u_cookie, "(b)", can_restart);
        g_variant_unref (u_cookie);

        return TRUE;


}

/**
 * egg_console_kit_can_suspend:
 **/
gboolean
egg_console_kit_can_suspend (EggConsoleKit *console, gboolean *can_suspend, GError **error)
{
	GError *error_local = NULL;
	gboolean ret;
	gchar  *retval;

	g_return_val_if_fail (EGG_IS_CONSOLE_KIT (console), FALSE);
	g_return_val_if_fail (console->priv->proxy_manager != NULL, FALSE);


        GVariant * u_cookie;
        u_cookie = g_dbus_proxy_call_sync (console->priv->proxy_manager,
                           "CanSuspend",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE, -1,
                           NULL, &error_local);

        if (u_cookie == NULL)
        {
                g_error_free (error_local);
                return FALSE;
        }
        g_variant_get (u_cookie, "(s)", retval);
        g_variant_unref (u_cookie);

	*can_suspend = g_strcmp0 (retval, "yes") == 0 ||
		       g_strcmp0 (retval, "challenge") == 0;

	g_free (retval);
	return TRUE;
}

/**
 * egg_console_kit_can_hibernate:
 **/

gboolean
egg_console_kit_can_hibernate (EggConsoleKit *console, gboolean *can_hibernate, GError **error)
{
	GError *error_local = NULL;
	gboolean ret;
	gchar  *retval;

	g_return_val_if_fail (EGG_IS_CONSOLE_KIT (console), FALSE);
	g_return_val_if_fail (console->priv->proxy_manager != NULL, FALSE);

        GVariant * u_cookie;
        u_cookie = g_dbus_proxy_call_sync (console->priv->proxy_manager,
                           "CanHibernate",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE, -1,
                           NULL, &error_local);

        if (u_cookie == NULL)
        {
                g_error_free (error_local);
                return FALSE;
        }
        g_variant_get (u_cookie, "(s)", retval);
        g_variant_unref (u_cookie);

	*can_hibernate = g_strcmp0 (retval, "yes") == 0 ||
		       g_strcmp0 (retval, "challenge") == 0;
	return TRUE;
}

/**
 * egg_console_kit_is_local:
 *
 * Return value: Returns whether the session is local
 **/
gboolean
egg_console_kit_is_local (EggConsoleKit *console)
{
	gboolean ret = FALSE;
	gboolean value = FALSE;
	GError *error = NULL;

	g_return_val_if_fail (EGG_IS_CONSOLE_KIT (console), FALSE);

	/* maybe console kit does not know about our session */
	if (console->priv->proxy_session == NULL) {
		egg_warning ("no ConsoleKit session");
		goto out;
	}


        GVariant * u_cookie;
        u_cookie = g_dbus_proxy_call_sync (console->priv->proxy_session,
                           "IsLocal",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE, -1,
                           NULL, &error);

        if (u_cookie == NULL)
        {
                g_error_free (error);
                return FALSE;
        }
        g_variant_get (u_cookie, "(b)", &value);
        g_variant_unref (u_cookie);


	/* return value only if we successed */
	ret = value;
out:
	return ret;
}

/**
 * egg_console_kit_is_active:
 *
 * Return value: Returns whether the session is active on the Seat that it is attached to.
 **/
gboolean
egg_console_kit_is_active (EggConsoleKit *console)
{
	gboolean ret = FALSE;
	gboolean value = FALSE;
	GError *error = NULL;

	g_return_val_if_fail (EGG_IS_CONSOLE_KIT (console), FALSE);

	/* maybe console kit does not know about our session */
	if (console->priv->proxy_session == NULL) {
		egg_warning ("no ConsoleKit session");
		goto out;
	}


        GVariant * u_cookie;
        u_cookie = g_dbus_proxy_call_sync (console->priv->proxy_session,
                           "IsActive",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE, -1,
                           NULL, &error);

        if (u_cookie == NULL)
        {
                g_error_free (error);
                return FALSE;
        }
        g_variant_get (u_cookie, "(b)", &value);
        g_variant_unref (u_cookie);


	/* return value only if we successed */
	ret = value;
out:
	return ret;
}

/**
 * egg_console_kit_active_changed_cb:
 **/
static void
egg_console_kit_active_changed_cb (GDBusProxy *proxy, gchar *sender_name, gchar * signal_name, GVariant *parameters, gpointer user_data)
{
        EggConsoleKit *console = EGG_CONSOLE_KIT(user_data);
	gboolean active;
        if(g_strcmp0(signal_name,"ActiveChanged") == 0) {
	        g_variant_get (parameters,"(b)",&active);
        	//egg_debug ("emitting active: %i", active);
		g_signal_emit (console, signals [EGG_CONSOLE_KIT_ACTIVE_CHANGED], 0, active);
	
        }
}

/**
 * egg_console_kit_class_init:
 * @klass: The EggConsoleKitClass
 **/
static void
egg_console_kit_class_init (EggConsoleKitClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = egg_console_kit_finalize;
	g_type_class_add_private (klass, sizeof (EggConsoleKitPrivate));
	signals [EGG_CONSOLE_KIT_ACTIVE_CHANGED] =
		g_signal_new ("active-changed",
			      G_TYPE_FROM_CLASS (object_class), G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (EggConsoleKitClass, active_changed),
			      NULL, NULL, g_cclosure_marshal_VOID__BOOLEAN,
			      G_TYPE_NONE, 1, G_TYPE_BOOLEAN);
}

/**
 * egg_console_kit_init:
 **/
static void
egg_console_kit_init (EggConsoleKit *console)
{
	gboolean ret;
	GError *error = NULL;
	guint32 pid;

	console->priv = EGG_CONSOLE_KIT_GET_PRIVATE (console);
	console->priv->proxy_manager = NULL;
	console->priv->session_id = NULL;

   console->priv->proxy_manager = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
                                      G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES,
                                      NULL,
                                      CONSOLEKIT_NAME,
                                      CONSOLEKIT_MANAGER_PATH,
                                      CONSOLEKIT_MANAGER_INTERFACE,
                                      NULL,
                                      &error);
   if (console->priv->proxy_manager == NULL) {
       g_printerr ("Could not connect to UPower system bus: %s", error->message);
       g_error_free (error);
       goto out;
   }


	/* get the session we are running in */
	pid = getpid ();

	GVariant *u_cookie;
   	u_cookie = g_dbus_proxy_call_sync (console->priv->proxy_manager,
                          "GetSessionForUnixProcess",
                          g_variant_new ("(u)",pid),
                          G_DBUS_CALL_FLAGS_NONE,
                          -1,
                          NULL,
                          &error);

	if (u_cookie == NULL)
	{
		g_error_free (error);
		return;
	}
	g_variant_get (u_cookie, "(o)", &console->priv->session_id);

	//egg_debug ("ConsoleKit session ID: %s", console->priv->session_id);

	

	/* connect to session */
	console->priv->proxy_session = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
					G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES,
					NULL,
					CONSOLEKIT_NAME,
					console->priv->session_id,
					CONSOLEKIT_SESSION_INTERFACE,
					NULL,
					&error);


	if (console->priv->proxy_session == NULL) {
		egg_warning ("cannot connect to: %s", console->priv->session_id);
		goto out;
	}

	g_signal_connect (console->priv->proxy_session, "g-signal", G_CALLBACK (egg_console_kit_active_changed_cb), console);

out:
	return;
}

/**
 * egg_console_kit_finalize:
 * @object: The object to finalize
 **/
static void
egg_console_kit_finalize (GObject *object)
{
	EggConsoleKit *console;

	g_return_if_fail (EGG_IS_CONSOLE_KIT (object));

	console = EGG_CONSOLE_KIT (object);

	g_return_if_fail (console->priv != NULL);
	if (console->priv->proxy_manager != NULL)
		g_object_unref (console->priv->proxy_manager);
	if (console->priv->proxy_session != NULL)
		g_object_unref (console->priv->proxy_session);
	g_free (console->priv->session_id);

	G_OBJECT_CLASS (egg_console_kit_parent_class)->finalize (object);
}

/**
 * egg_console_kit_new:
 *
 * Return value: a new EggConsoleKit object.
 **/
EggConsoleKit *
egg_console_kit_new (void)
{
	if (egg_console_kit_object != NULL) {
		g_object_ref (egg_console_kit_object);
	} else {
		egg_console_kit_object = g_object_new (EGG_TYPE_CONSOLE_KIT, NULL);
		g_object_add_weak_pointer (egg_console_kit_object, &egg_console_kit_object);
	}

	return EGG_CONSOLE_KIT (egg_console_kit_object);
}

