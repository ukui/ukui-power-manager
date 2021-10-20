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

#include "upowerinfo.h"

UPowerInfo::UPowerInfo(QObject *parent) : QObject(parent)
{
    iface = new QDBusInterface(UPOWER_SERVICE,UPOWER_PATH,
                             FREEDESKTOP_UPOWER,
                             QDBusConnection::systemBus());

    QDBusConnection::systemBus().connect(UPOWER_SERVICE,UPOWER_PATH,
                                          FREEDESKTOP_UPOWER,
                                          "PropertiesChanged",
                                          this,
                                          SLOT(dealMessage(void))
                                          );
    mOnBattery = OnBattery();
    mLidIsClosed = LidIsClosed();
}

UPowerInfo::~UPowerInfo()
{
    delete iface;
}

QString UPowerInfo::UPowerVersion()
{
    return "3.1";
}

QString UPowerInfo::CanHibernate()
{
   QDBusInterface interface("org.freedesktop.login1", "/org/freedesktop/login1",
                               "org.freedesktop.login1.Manager",
                               QDBusConnection::systemBus());
      if (!interface.isValid()) {
          qCritical() << QDBusConnection::sessionBus().lastError().message();
      }
      /*调用远程的 CanHibernate 方法，判断是否可以执行休眠的操作,返回值为yes为允许执行休眠，no为无法执行休眠 na为交换分区不足*/
      QDBusReply<QString> reply = interface.call("CanHibernate");
      if (reply.isValid()) {
          return reply.value();
      } else {
          return "Call Dbus method failed";
      }
}

QString UPowerInfo::MachineType()
{
    QDBusReply<QVariant> reply = iface->call("Get",UPOWER_SERVICE,"LidIsPresent");
    if(reply.isValid()){
        if(true == reply.value().toBool()){
            return "book";
        }
        else{
            return "pc";
        }
    }else{
        return "get lid is present erroy!";
    }
}

bool UPowerInfo::OnBattery()
{
//    QDBusInterface iface(UPOWER_SERVICE,UPOWER_PATH,
//                         FREEDESKTOP_UPOWER,
//                         QDBusConnection::systemBus());
    QDBusReply<QVariant> reply = iface->call("Get",UPOWER_SERVICE,"OnBattery");
    if(reply.isValid()){
        return reply.value().toBool();
    }else {
        return false;
    }
}

bool UPowerInfo::LidIsClosed()
{
    QDBusReply<QVariant> reply = iface->call("Get",UPOWER_SERVICE,"LidIsClosed");
    if(reply.isValid()){
       return reply.value().toBool();
    }else {
       return false;
    }
}

QString UPowerInfo::DaemonVersion()
{
//    QDBusInterface iface(UPOWER_SERVICE,UPOWER_PATH,
//                         FREEDESKTOP_UPOWER,
//                         QDBusConnection::systemBus());
    QDBusReply<QVariant> reply = iface->call("Get",UPOWER_SERVICE,"DaemonVersion");
    if(reply.isValid()){
         return reply.value().toString();
    }else {
         return "get version erroy!";
    }
}

void UPowerInfo::dealMessage()
{
    dealPowerMessage();
    dealLidMessage();
}

void UPowerInfo::dealPowerMessage()
{
    bool value;
    value = OnBattery();
    if(mOnBattery != value){
        mOnBattery = value;
        emit acChanged(mOnBattery);
    }
}

void UPowerInfo::dealLidMessage()
{
    bool value;
    value = LidIsClosed();
    if(mLidIsClosed != value){
        mLidIsClosed = value;
        emit lidChanged(mLidIsClosed);
    }
}



