/*
 * Copyright: 2021, KylinSoft Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU  Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or (at your option)
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

#include "powermsgnotificat.h"

PowerMsgNotificat::PowerMsgNotificat() {}

PowerMsgNotificat::~PowerMsgNotificat() {}

void PowerMsgNotificat::initPowerMsgNotificat()
{
    QDBusConnection::systemBus().connect(
        QString(),
        QString("/"),
        QString("org.ukui.powermanagement.interface"),
        QString("CanControl"),
        this,
        SLOT(msgNotification(QString)));
    //    QDBusConnection::sessionBus().connect(UKUI_UPOWER_SERVICE,UKUI_UPOWER_PATH,
    //                                          UKUI_UPOWER_INTERFACE,
    //                                          "LowBatteryState",
    //                                          this,SLOT(dealLowBatteryNotify(bool))
    //                                          );
}

void PowerMsgNotificat::msgNotification(QString msg)
{
    QString mType = tr("error message");
    notifySend(mType, msg);
}

// void PowerMsgNotificat::dealLowBatteryNotify(bool state)
//{
//    if(state){
//        QString mType = "低电量通知";
//        notifySend(mType,"系统进入低电量状态");
//    }
//    qDebug()<<"low power notify";
//}

void PowerMsgNotificat::notifySend(const QString &type, const QString &arg)
{
    QDBusInterface iface(
        "org.freedesktop.Notifications",
        "/org/freedesktop/Notifications",
        "org.freedesktop.Notifications",
        QDBusConnection::sessionBus());
    QList<QVariant> args;
    QStringList argg;
    QMap<QString, QVariant> pearMap;
    args << tr("电源管理") << ((unsigned int)0) << QString("ukui-power-manager")
         << type //显示的是什么类型的信息//系统升级
         << arg  //显示的具体信息
         << argg << pearMap << (int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect, "Notify", args);
}
