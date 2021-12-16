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


#include "eventwatcher.h"

EventWatcher::EventWatcher()
{
    const QByteArray id(POWER_MANAGER_SETTINGS);
    mPowerManagementGsettings = new QGSettings(id);
}

EventWatcher::~EventWatcher()
{
    delete mPowerManagementGsettings;
}

void EventWatcher::initEventWatcher()
{
    getUserBrightness();
    setBrightness();
    connect(mPowerManagementGsettings, &QGSettings::changed, this, [=](const QString &key) {
        if (key == BRIGHTNESS_AC) {
            getUserBrightness();
            setBrightness();
        }
    });
}

void EventWatcher::getUserBrightness()
{
    mMaxBrightness = controlPowerManagement(GET_MAX_BRIGHTNESS);
    qDebug() << "max brightness is :" << mMaxBrightness;
    if (10.0 > mPowerManagementGsettings->get(BRIGHTNESS_AC).toDouble()) {
        mUserBrightness = 10.0;
    } else {
        mUserBrightness = mPowerManagementGsettings->get(BRIGHTNESS_AC).toDouble() * 0.01 * mMaxBrightness;
    }

    qDebug() << "user brightness is :" << mUserBrightness;
}

void EventWatcher::setBrightness()
{
    controlPowerManagement(REGULATE_BRIGHTNESS, mUserBrightness);
}

void EventWatcher::turnOffDisplay()
{
    if (mPowerManagementGsettings->get(LOCK_BLANK_SCREEN).toBool()) {
        lock();
        qDebug()<<"lock success";
    }
    //调用关闭显示器的接口
    if ("wayland" == qgetenv("XDG_SESSION_TYPE")) {
        system("export QT_QPA_PLATFORM=wayland && kscreen-doctor --dpms off");
    } else {
        system("xset dpms force off");
    }
    qDebug() << "The display is turned off" << qgetenv("XDG_SESSION_TYPE");
}

void EventWatcher::turnOnDisplay()
{
    if ("wayland" == qgetenv("XDG_SESSION_TYPE")) {
        system("export QT_QPA_PLATFORM=wayland && kscreen-doctor --dpms on");
    } else {
        system("xset dpms force on");
    }
    qDebug() << "The display is turned on" << qgetenv("XDG_SESSION_TYPE");
}

void EventWatcher::lock()
{
    system("ukui-screensaver-command -l");
    qDebug() << "lock success";
}

void EventWatcher::suspend()
{
    controlPowerManagement(SUSPEND);
    qDebug() << "suspend success";
}

void EventWatcher::hibernate()
{
    controlPowerManagement(HIBERNATE);
    qDebug() << "hibernate success";
}

void EventWatcher::powerOff()
{
    controlPowerManagement(POWER_OFF);
    qDebug() << "power off success";
}

//此处代码后期可以进行优化
int EventWatcher::controlPowerManagement(const QString &type)
{
    QDBusInterface iface(
        POWER_MANAGEMENT_SERVICE, POWER_MANAGEMENT_PATH, POWER_MANAGEMENT_INTERFACE_NAME, QDBusConnection::systemBus());
    QDBusReply<qulonglong> reply = iface.call(type);
    return reply.value();
}

void EventWatcher::controlPowerManagement(const QString &type, const qulonglong &brightness)
{
    QDBusInterface iface(
        POWER_MANAGEMENT_SERVICE, POWER_MANAGEMENT_PATH, POWER_MANAGEMENT_INTERFACE_NAME, QDBusConnection::systemBus());
    iface.call(type, brightness);
}

void EventWatcher::controlPowerManagement(const QString &type, const int &value)
{
    QDBusInterface iface(
        POWER_MANAGEMENT_SERVICE, POWER_MANAGEMENT_PATH, POWER_MANAGEMENT_INTERFACE_NAME, QDBusConnection::systemBus());
    iface.call(type, value);
}

void EventWatcher::controlPowerManagement(const QString &type, const QString &contrl)
{
    QDBusInterface iface(
        POWER_MANAGEMENT_SERVICE, POWER_MANAGEMENT_PATH, POWER_MANAGEMENT_INTERFACE_NAME, QDBusConnection::systemBus());
    iface.call(type, contrl);
}
