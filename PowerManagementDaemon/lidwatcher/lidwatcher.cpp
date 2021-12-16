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


#include "lidwatcher.h"

LidWatcher::LidWatcher()
{

}

LidWatcher::~LidWatcher() {}

void LidWatcher::initLidWatcher()
{
    inhibitSystemdLogin();
    QDBusConnection::systemBus().connect(
        UPOWER_SERVICE, UPOWER_PATH, FREEDESKTOP_UPOWER, "PropertiesChanged", this, SLOT(dealLidWatcherMssage(void)));
    displayState = true;
}

void LidWatcher::inhibitSystemdLogin()
{
    QDBusInterface manager(
        QStringLiteral("org.freedesktop.login1"),
        QStringLiteral("/org/freedesktop/login1"),
        QStringLiteral("org.freedesktop.login1.Manager"),
        QDBusConnection::systemBus());
    QDBusReply<QDBusUnixFileDescriptor> reply = manager.call(
        QStringLiteral("Inhibit"),
        QStringLiteral("handle-lid-switch"),
        QStringLiteral("lxqt-powermanagment"),
        QStringLiteral("LidWatcher is in da house!"),
        QStringLiteral("block"));
    if (reply.isValid()) {
        logindLock = reply.value();
        qDebug() << "Inhibit got:" << logindLock.fileDescriptor();
    } else {
        qDebug() << "Error from inhibit:" << reply.error();
    }
}

void LidWatcher::dealLidWatcherMssage(void)
{
    QString contrl;
    QDBusInterface iface(UPOWER_SERVICE, UPOWER_PATH, FREEDESKTOP_UPOWER, QDBusConnection::systemBus());
    QDBusReply<QVariant> reply = iface.call("Get", "org.freedesktop.UPower", "LidIsClosed");
    if (reply.isValid()) {
        qDebug() << "lid state:" << reply.value();
        contrl = mPowerManagementGsettings->get(BUTTON_LID_AC).toString();
        qDebug() << "lid close contrl：" << contrl;
        bool value = reply.value().toBool();
        if (true == value) {
            if ("blank" == contrl) {
                turnOffDisplay();
                displayState = false;
            } else if ("suspend" == contrl) {
                suspend();
            } else if ("shutdown" == contrl) {
                powerOff();
            } else if ("hibernate" == contrl) {
                hibernate();
            }
        } else {
            if ("blank" == contrl && false == displayState) {
                turnOnDisplay();
                displayState = true;
            }
        }
    } else {
        qDebug() << "Failed to get lid closed event!";
    }
}
