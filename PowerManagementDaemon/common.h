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
#include <QProcess>

//gsettings 配置
#define POWER_MANAGER_SETTINGS   "org.ukui.power-manager"
//#define SLEEP_COMPUTER_BAT_KEY   "sleepComputerBattery"
#define SLEEP_DISPLAY_AC_KEY     "sleepDisplayAc"
#define SLEEP_DISPLAY_BAT_KEY    "sleepDisplayBattery"   //空闲关闭显示器
#define SLEEP_COMPUTER_AC        "sleepComputerAc"
#define SLEEP_COMPUTER_BATTERY   "sleepComputerBattery"  //空闲随睡
#define LOCK_BLANK_SCREEN        "lockBlankScreen"
#define IDLE_DIM_TIME            "idleDimTime"
#define BRIGHTNESS_AC            "brightnessAc"          //亮度值
#define BRIGHTNESS_BATTERY       "brightnessBattery"
#define BUTTON_LID_AC            "buttonLidAc"           //盒盖事件触发操作
#define BUTTON_LID_BATTERY       "buttonLidBattery"
#define POWER_POLICY_AC          "powerPolicyAc"         //电源策略(0:性能 1:平衡 2:节能)
#define POWER_POLICY_BATTERY     "powerPolicyBattery"
#define ON_BATTERY_AUTO_SAVE     "onBatteryAutoSave"     //电池自动开启节能模式
#define LOW_BATTERY_ATUO_SAVE    "lowBatteryAutoSave"    //低电量时自动开启节能模式
#define PERCENTAGE_LOW           "percentageLow"         //低电量百分百通知
#define PERCENTAGE_ACTION        "percentageAction"      //极低电量
#define ACTION_CRITICAL_BATTERY  "actionCriticalBattery" //极低电量时执行



//dbus 配置
#define GNOME_SESSION_MANAGER               "org.gnome.SessionManager.Presence"
#define SESSION_MANAGER_PATH                "/org/gnome/SessionManager/Presence"
#define POWER_MANAGEMENT_SERVICE            "org.ukui.powermanagement"
#define POWER_MANAGEMENT_PATH               "/"
#define POWER_MANAGEMENT_INTERFACE_NAME     "org.ukui.powermanagement.interface"
#define FREEDESKTOP_UPOWER                  "org.freedesktop.DBus.Properties"
#define UKUI_UPOWER_PATH                    "/"
#define UKUI_UPOWER_SERVICE                 "org.ukui.upower"
#define UKUI_UPOWER_INTERFACE               "org.ukui.upower"
#define UPOWER_PATH                         "/org/freedesktop/UPower"
#define UPOWER_SERVICE                      "org.freedesktop.UPower"
#define UPOWER_DISPLAY_PATH                 "/org/freedesktop/UPower/devices/DisplayDevice"
#define SETTINGS_DAEMON_MEDIAKEYS           "org.ukui.SettingsDaemon.MediaKeys"
#define SETTINGS_DAEMON_MEDIAKEYS_PATH      "/org/ukui/SettingsDaemon/MediaKeys"

//电源管理dbus接口
#define CPU_FREQENCY_MODULATION     "CpuFreqencyModulation"
#define GET_BRIGHTNESS              "GetBrightness"
#define GET_MAX_BRIGHTNESS          "GetMaxBrightness"
#define GPU_FREQENCY_MODULATION     "GpuFreqencyModulation"
#define HIBERNATE                   "Hibernate"
#define LOCK_SCREEN                 "LockScreen"
#define POWER_OFF                   "PowerOff"
#define REBOOT                      "Reboot"
#define REGULATE_BRIGHTNESS         "RegulateBrightness"
#define SUSPEND                     "Suspend"
#define TURN_OFF_DISPLAY            "TurnOffDisplay"
#define EXIT_SERVICE                "ExitService"


#endif // COMMON_H
