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

#include "acwatcher.h"

AcWatcher::AcWatcher()
{
    // qDebug() << "AcWatcher";
    // initAcWatcher();
}

AcWatcher::~AcWatcher() {}

void AcWatcher::initAcWatcher()
{
    QDBusConnection::systemBus().connect(
        UPOWER_SERVICE, UPOWER_PATH, FREEDESKTOP_UPOWER, "PropertiesChanged", this, SLOT(dealAcWatcherMssage(void)));
    mOnBattery = readOnBattery();
    // qDebug()<<"mOnBattery:"<<mOnBattery;
}

void AcWatcher::dealAcWatcherMssage()
{
    bool value;
    value = readOnBattery();
    // qDebug()<<"当前onBattery:"<<mOnBattery;
    if (mOnBattery != value) {
        mOnBattery = value;
        emit acChanged(mOnBattery);
        qDebug() << "Power status change signal transmission";
    }
}

bool AcWatcher::readOnBattery()
{
    bool value;
    QDBusInterface iface(UPOWER_SERVICE, UPOWER_PATH, FREEDESKTOP_UPOWER, QDBusConnection::systemBus());
    QDBusReply<QVariant> reply = iface.call("Get", "org.freedesktop.UPower", "OnBattery");
    if (reply.isValid()) {
        value = reply.value().toBool();
        // qDebug() << "使用电池:" << value;
    } else {
        qDebug() << "OnBattery error!";
    }
    return value;
}
