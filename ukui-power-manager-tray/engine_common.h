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

#ifndef ENGINECOMMON_H
#define ENGINECOMMON_H

#define	GPM_DBUS_SERVICE		"org.ukui.PowerManager"
#define	GPM_DBUS_INTERFACE		"org.ukui.PowerManager"
#define	GPM_DBUS_INTERFACE_BACKLIGHT	"org.ukui.PowerManager.Backlight"
#define	GPM_DBUS_INTERFACE_KBD_BACKLIGHT	"org.ukui.PowerManager.KbdBacklight"
#define	GPM_DBUS_PATH			"/org/ukui/PowerManager"
#define	GPM_DBUS_PATH_BACKLIGHT		"/org/ukui/PowerManager/Backlight"
#define GPM_DBUS_PATH_KBD_BACKLIGHT    "/org/ukui/PowerManager/KbdBacklight"

/* common descriptions of this program */
#define GPM_NAME 			_("Power Manager")
#define GPM_DESCRIPTION 		_("Power Manager for the MATE desktop")

/* schema location */
#define GPM_SETTINGS_SCHEMA		"org.ukui.power-manager"
#define GPM_SETTINGS_KEY_POLICY         "icon-policy"

/* actions */
#define GPM_SETTINGS_ACTION_CRITICAL_UPS		"action-critical-ups"
#define GPM_SETTINGS_ACTION_CRITICAL_BATT		"action-critical-battery"
#define GPM_SETTINGS_ACTION_LOW_UPS			"action-low-ups"
#define GPM_SETTINGS_ACTION_SLEEP_TYPE_AC		"action-sleep-type-ac"
#define GPM_SETTINGS_ACTION_SLEEP_TYPE_BATT		"action-sleep-type-battery"
#define GPM_SETTINGS_SLEEP_WHEN_CLOSED			"event-when-closed-battery"

/* backlight stuff */
#define GPM_SETTINGS_BACKLIGHT_ENABLE			"backlight-enable"
#define GPM_SETTINGS_BACKLIGHT_BATTERY_REDUCE		"backlight-battery-reduce"
#define GPM_SETTINGS_DPMS_METHOD_AC			"dpms-method-ac"
#define GPM_SETTINGS_DPMS_METHOD_BATT			"dpms-method-battery"
#define GPM_SETTINGS_IDLE_BRIGHTNESS			"idle-brightness"
#define GPM_SETTINGS_IDLE_DIM_AC			"idle-dim-ac"
#define GPM_SETTINGS_IDLE_DIM_BATT			"idle-dim-battery"
#define GPM_SETTINGS_IDLE_DIM_TIME			"idle-dim-time"
#define GPM_SETTINGS_BRIGHTNESS_AC			"brightness-ac"
#define GPM_SETTINGS_BRIGHTNESS_BAT			"brightness-bat"
#define GPM_SETTINGS_BRIGHTNESS_DIM_BATT		"brightness-dim-battery"

/* keyboard backlight */
#define GPM_SETTINGS_KBD_BACKLIGHT_BATT_REDUCE     "kbd-backlight-battery-reduce"
#define GPM_SETTINGS_KBD_BRIGHTNESS_ON_AC      "kbd-brightness-on-ac"
#define GPM_SETTINGS_KBD_BRIGHTNESS_DIM_BY_ON_BATT      "kbd-brightness-dim-by-on-battery"
#define GPM_SETTINGS_KBD_BRIGHTNESS_DIM_BY_ON_IDLE "kbd-brightness-dim-by-on-idle"

/* buttons */
#define GPM_SETTINGS_BUTTON_LID_AC			"button-lid-ac"
#define GPM_SETTINGS_BUTTON_LID_BATT			"button-lid-battery"
#define GPM_SETTINGS_BUTTON_SUSPEND			"button-suspend"
#define GPM_SETTINGS_BUTTON_HIBERNATE			"button-hibernate"
#define GPM_SETTINGS_BUTTON_POWER			"button-power"

/* general */
#define GPM_SETTINGS_USE_TIME_POLICY			"use-time-for-policy"
#define GPM_SETTINGS_NETWORKMANAGER_SLEEP		"network-sleep"
#define GPM_SETTINGS_IDLE_CHECK_CPU			"check-type-cpu"


/* notify */
#define GPM_SETTINGS_NOTIFY_LOW_CAPACITY		"notify-low-capacity"
#define GPM_SETTINGS_NOTIFY_DISCHARGING			"notify-discharging"
#define GPM_SETTINGS_NOTIFY_FULLY_CHARGED		"notify-fully-charged"
#define GPM_SETTINGS_NOTIFY_SLEEP_FAILED		"notify-sleep-failed"
#define GPM_SETTINGS_NOTIFY_SLEEP_FAILED_URI		"notify-sleep-failed-uri"
#define GPM_SETTINGS_NOTIFY_LOW_POWER			"notify-low-power"
#define GPM_SETTINGS_BAT_POLICY                         "power-policy-battery"
#define GPM_SETTINGS_ON_BAT_AUTO_SAVE                           "on-battery-auto-save"
#define GPM_SETTINGS_LOW_BAT_AUTO_SAVE                          "low-battery-auto-save"
#define GPM_SETTINGS_DISPLAY_LEFT_TIME                          "dispaly-left-time-of-charge-and-discharge"


/* thresholds */
#define GPM_SETTINGS_PERCENTAGE_LOW			"percentage-low"
#define GPM_SETTINGS_PERCENTAGE_CRITICAL		"percentage-critical"
#define GPM_SETTINGS_PERCENTAGE_ACTION			"percentage-action"
#define GPM_SETTINGS_TIME_LOW				"time-low"
#define GPM_SETTINGS_TIME_CRITICAL			"time-critical"
#define GPM_SETTINGS_TIME_ACTION			"time-action"

/* timeout */
#define GPM_SETTINGS_SLEEP_COMPUTER_AC			"sleep-computer-ac"
#define GPM_SETTINGS_SLEEP_COMPUTER_BATT		"sleep-computer-battery"
#define GPM_SETTINGS_SLEEP_COMPUTER_UPS			"sleep-computer-ups"
#define GPM_SETTINGS_SLEEP_DISPLAY_AC			"sleep-display-ac"
#define GPM_SETTINGS_SLEEP_DISPLAY_BATT			"sleep-display-battery"
#define GPM_SETTINGS_SLEEP_DISPLAY_UPS			"sleep-display-ups"

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

#if __aarch64__
#define ARCH64                                  TRUE
#else
#define ARCH64                                  FALSE
#endif

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
    UP_DEVICE_KIND_UNKNOWN,
    UP_DEVICE_KIND_LINE_POWER,
    UP_DEVICE_KIND_BATTERY,
    UP_DEVICE_KIND_UPS,
    UP_DEVICE_KIND_MONITOR,
    UP_DEVICE_KIND_MOUSE,
    UP_DEVICE_KIND_KEYBOARD,
    UP_DEVICE_KIND_PDA,
    UP_DEVICE_KIND_PHONE,
    UP_DEVICE_KIND_MEDIA_PLAYER,
    UP_DEVICE_KIND_TABLET,
    UP_DEVICE_KIND_COMPUTER,
    UP_DEVICE_KIND_LAST
} UpDeviceKind;

/**
 * UpDeviceState:
 *
 * The device state.
 **/
typedef enum {
    UP_DEVICE_STATE_UNKNOWN,
    UP_DEVICE_STATE_CHARGING,
    UP_DEVICE_STATE_DISCHARGING,
    UP_DEVICE_STATE_EMPTY,
    UP_DEVICE_STATE_FULLY_CHARGED,
    UP_DEVICE_STATE_PENDING_CHARGE,
    UP_DEVICE_STATE_PENDING_DISCHARGE,
    UP_DEVICE_STATE_LAST
} UpDeviceState;

/**
 * UpDeviceTechnology:
 *
 * The device technology.
 **/
typedef enum {
    UP_DEVICE_TECHNOLOGY_UNKNOWN,
    UP_DEVICE_TECHNOLOGY_LITHIUM_ION,
    UP_DEVICE_TECHNOLOGY_LITHIUM_POLYMER,
    UP_DEVICE_TECHNOLOGY_LITHIUM_IRON_PHOSPHATE,
    UP_DEVICE_TECHNOLOGY_LEAD_ACID,
    UP_DEVICE_TECHNOLOGY_NICKEL_CADMIUM,
    UP_DEVICE_TECHNOLOGY_NICKEL_METAL_HYDRIDE,
    UP_DEVICE_TECHNOLOGY_LAST
} UpDeviceTechnology;

/**
 * UpDeviceLevel:
 *
 * The warning level of a battery.
 **/
typedef enum {
    UP_DEVICE_LEVEL_UNKNOWN,
    UP_DEVICE_LEVEL_NONE,
    UP_DEVICE_LEVEL_DISCHARGING,
    UP_DEVICE_LEVEL_LOW,
    UP_DEVICE_LEVEL_CRITICAL,
    UP_DEVICE_LEVEL_ACTION,
    UP_DEVICE_LEVEL_LAST
} UpDeviceLevel;

#endif // ENGINECOMMON_H
