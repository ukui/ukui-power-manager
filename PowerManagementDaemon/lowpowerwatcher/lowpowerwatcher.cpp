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


#include "lowpowerwatcher.h"

LowPowerWatcher::LowPowerWatcher()
{

}

LowPowerWatcher::~LowPowerWatcher()
{
    delete mContrlTimer;
}

void LowPowerWatcher::initLowPowerWatcher()
{
    mContrlTimer = new QTimer;
    initConnect();
    mPercentageLow = readPercentage();
    if (mSetPercentageLow > mPercentageLow) {
        mLowPowerState = true;
    } else {
        mLowPowerState = false;
    }
    mNotifyState = false;
}

void LowPowerWatcher::initConnect()
{
    readSettings();
    connect(mPowerManagementGsettings, &QGSettings::changed, this, [=](const QString &key) {
        readSettings();
        if (key == PERCENTAGE_ACTION || key == ACTION_CRITICAL_BATTERY) {
            veryLowBatteryContrlStop();
        }
    });
    QDBusConnection::sessionBus().connect(
        UKUI_UPOWER_SERVICE,
        UKUI_UPOWER_PATH,
        UKUI_UPOWER_INTERFACE,
        "PowerState",
        this,
        SLOT(dealPowerStateMssage(bool)));
    QDBusConnection::sessionBus().connect(
        UKUI_UPOWER_SERVICE,
        UKUI_UPOWER_PATH,
        UKUI_UPOWER_INTERFACE,
        "LowBatteryState",
        this,
        SLOT(dealLowBatteryMssage(bool)));
    QDBusConnection::sessionBus().connect(
        UKUI_UPOWER_SERVICE,
        UKUI_UPOWER_PATH,
        UKUI_UPOWER_INTERFACE,
        "VeryLowBatteryState",
        this,
        SLOT(dealVeryLowBatteryMssage(bool)));
    connect(mContrlTimer, &QTimer::timeout, [this] {
        veryLowBatteryContrl(mActionCriticalBattery);
        mContrlTimer->stop();
    });
}

void LowPowerWatcher::readSettings()
{
    mSetPercentageLow = mPowerManagementGsettings->get(PERCENTAGE_LOW).toInt();
    qDebug() << "mSetPercentageLow:" << mSetPercentageLow;
    mPercentageAction = mPowerManagementGsettings->get(PERCENTAGE_ACTION).toInt();
    qDebug() << "mPercentageAction:" << mPercentageAction;
    mActionCriticalBattery = mPowerManagementGsettings->get(ACTION_CRITICAL_BATTERY).toString();
    qDebug() << "mActionCriticalBattery:" << mActionCriticalBattery;
}

double LowPowerWatcher::readPercentage()
{
    double value;
    QDBusInterface iface(UPOWER_SERVICE, UPOWER_DISPLAY_PATH, FREEDESKTOP_UPOWER, QDBusConnection::systemBus());
    QDBusReply<QVariant> reply = iface.call("Get", "org.freedesktop.UPower.Device", "Percentage");
    if (reply.isValid()) {
        value = reply.value().toDouble();
    } else {
         qDebug() << "Power percentage get error!";
    }
    return value;
}

void LowPowerWatcher::dealLowBatteryMssage(bool state)
{
    mLowPowerState = state;
    if (mLowPowerState) {
        lowBatteryNotify();
    }
}

void LowPowerWatcher::dealVeryLowBatteryMssage(bool state)
{
    if (state && !mNotifyState) {
        veryLowBatteryNotify(mActionCriticalBattery);
    }
}

void LowPowerWatcher::dealPowerStateMssage(bool state)
{
    if (!state) {
        veryLowBatteryContrlStop();
    }
}

bool LowPowerWatcher::lowPowerState()
{
    return mLowPowerState;
}

void LowPowerWatcher::lowBatteryNotify()
{
    mPercentageLow = readPercentage();
    if (mPercentageLow > mPercentageAction) {
        QString mType = tr("低电量通知");
        notifySend(mType, "系统进入低电量状态");
        qDebug() << "low battery notify send success";
    }
}

void LowPowerWatcher::veryLowBatteryNotify(const QString &msg)
{
    mPercentageLow = readPercentage();
    QString mType = tr("极低电量通知");
    if ("blank" == msg) {
        notifySend(mType, QString("当前电量为：%1%，系统将于一分钟后关闭显示器").arg(mPercentageLow));
    } else if ("suspend" == msg) {
        notifySend(mType, QString("当前电量为：%1%，系统将于一分钟后睡眠").arg(mPercentageLow));
    } else if ("shutdown" == msg) {
        notifySend(mType, QString("当前电量为：%1%，系统将于一分钟后关机").arg(mPercentageLow));
    } else if ("hibernate" == msg) {
        notifySend(mType, QString("当前电量为：%1%，系统将于一分钟后休眠").arg(mPercentageLow));
    }
    mNotifyState = true;
    mContrlTimer->start(60000);
}

void LowPowerWatcher::veryLowBatteryContrl(const QString &contrl)
{
    if ("blank" == contrl) {
        turnOffDisplay();
    } else if ("suspend" == contrl) {
        suspend();
    } else if ("shutdown" == contrl) {
        powerOff();
    } else if ("hibernate" == contrl) {
        hibernate();
    }
    mNotifyState = false;
}

void LowPowerWatcher::veryLowBatteryContrlStop()
{
    if (mContrlTimer->isActive()) {
        mContrlTimer->stop();
        mNotifyState = false;
    }
}

void LowPowerWatcher::notifySend(const QString &type, const QString &arg)
{
    QDBusInterface iface(
        "org.freedesktop.Notifications",
        "/org/freedesktop/Notifications",
        "org.freedesktop.Notifications",
        QDBusConnection::sessionBus());
    QList<QVariant> args;
    QStringList argg;
    QMap<QString, QVariant> pear_map;
    args << tr("电源管理") << ((unsigned int)0) << QString("ukui-power-manager")
         << type //显示的是什么类型的信息//系统升级
         << arg  //显示的具体信息
         << argg << pear_map << (int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect, "Notify", args);
}
