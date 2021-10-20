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
