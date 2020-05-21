/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */
#ifndef STATISTICSCOMMON_H
#define STATISTICSCOMMON_H

#include <QString>
#include "customtype.h"

#define	GPM_DBUS_SERVICE		"org.ukui.PowerManager"
#define	GPM_DBUS_INTERFACE		"org.ukui.PowerManager"
#define	GPM_DBUS_PATH			"/org/ukui/PowerManager"

/* common descriptions of this program */
#define GPM_NAME 			_("Power Manager")
#define GPM_DESCRIPTION 		_("Power Manager for the UKUI desktop")

/* schema location */
#define GPM_SETTINGS_SCHEMA				"org.ukui.power-manager"

/* ui */
#define GPM_SETTINGS_ICON_POLICY			"icon-policy"
#define GPM_SETTINGS_ENABLE_SOUND			"enable-sound"
#define GPM_SETTINGS_SHOW_ACTIONS			"show-actions"

/* statistics */
#define GPM_SETTINGS_INFO_HISTORY_TIME			"info-history-time"
#define GPM_SETTINGS_INFO_HISTORY_TYPE			"info-history-type"
#define GPM_SETTINGS_INFO_HISTORY_GRAPH_SMOOTH		"info-history-graph-smooth"
#define GPM_SETTINGS_INFO_HISTORY_GRAPH_POINTS		"info-history-graph-points"
#define GPM_SETTINGS_INFO_STATS_TYPE			"info-stats-type"
#define GPM_SETTINGS_INFO_STATS_GRAPH_SMOOTH		"info-stats-graph-smooth"
#define GPM_SETTINGS_INFO_STATS_GRAPH_POINTS		"info-stats-graph-points"
#define GPM_SETTINGS_INFO_PAGE_NUMBER			"info-page-number"
#define GPM_SETTINGS_INFO_LAST_DEVICE			"info-last-device"

typedef enum {
    GPM_ICON_POLICY_ALWAYS,
    GPM_ICON_POLICY_PRESENT,
    GPM_ICON_POLICY_CHARGE,
    GPM_ICON_POLICY_LOW,
    GPM_ICON_POLICY_CRITICAL,
    GPM_ICON_POLICY_NEVER
} GpmIconPolicy;

typedef enum {
    GPM_ACTION_POLICY_BLANK,
    GPM_ACTION_POLICY_SUSPEND,
    GPM_ACTION_POLICY_SHUTDOWN,
    GPM_ACTION_POLICY_HIBERNATE,
    GPM_ACTION_POLICY_INTERACTIVE,
    GPM_ACTION_POLICY_NOTHING
} GpmActionPolicy;

typedef enum {
    GPM_GRAPH_WIDGET_TYPE_INVALID,
    GPM_GRAPH_WIDGET_TYPE_PERCENTAGE,
    GPM_GRAPH_WIDGET_TYPE_FACTOR,
    GPM_GRAPH_WIDGET_TYPE_TIME,
    GPM_GRAPH_WIDGET_TYPE_POWER,
    GPM_GRAPH_WIDGET_TYPE_VOLTAGE,
    GPM_GRAPH_WIDGET_TYPE_UNKNOWN
} GpmGraphWidgetType;

struct DEV
{
    UpDeviceKind kind;
    QString Device;
    QString Type;
    QString PowerSupply;
    QString Online;
    QString Vendor;
    QString Model;
    QString Refresh;
    QString Energy;
    QString EnergyEmpty;
    QString EnergyFull;
    QString EnergyFullDesign;
    QString EnergyRate;
    QString IsPresent;
    QString IsRechargeable;
    QString Percentage;
    QString State;
    QString TimeToEmpty;
    QString TimeToFull;
    QString Voltage;
    QString Capacity;
    QString update_time;
    QString serial;
    QString Technology;
    bool hasHistory;
    bool hasStat;
    QString path;

    GpmGraphWidgetType type_x;
    GpmGraphWidgetType type_y;
    bool autorange_x;
    int start_x;
    int stop_x;
    bool autorange_y;
    int start_y;
    int stop_y;
};

#endif // STATISTICSCOMMON_H
