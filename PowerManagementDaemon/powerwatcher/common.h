/*
 * Copyright 2021 KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COMMON_H
#define COMMON_H

//Qt
#include <QDebug>
#include <QtDBus>
#include <QtDBus/QDBusConnection>

//gsettings 配置
#define POWER_MANAGER_SETTINGS   "org.ukui.power-manager"
#define SLEEP_COMPUTER_BAT_KEY   "sleepComputerBattery"
#define SLEEP_DISPLAY_AC_KEY     "sleepDisplayAc"
#define SLEEP_DISPLAY_BAT_KEY    "sleepDisplayBattery"
#define LOCK_BLANK_SCREEN        "lockBlankScreen"
#define IDLE_DIM_TIME            "idleDimTime"


//dbus 配置
#define GNOME_SESSION_MANAGER "org.gnome.SessionManager.Presence"
#define SESSION_MANAGER_PATH  "/org/gnome/SessionManager/Presence"
#define POWER_MANAGEMENT_SERVICE "org.ukui.powermanagement"
#define POWER_MANAGEMENT_PATH "/"
#define POWER_MANAGEMENT_INTERFACE_NAME "org.ukui.powermanagement.interface"







#endif // COMMON_H
