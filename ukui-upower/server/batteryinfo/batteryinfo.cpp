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


#include <QDebug>
#include "batteryinfo.h"

BatteryInfo::BatteryInfo(QObject *parent) : QObject(parent)
{
    const QByteArray id("org.ukui.power-manager");
    mPowerManagementGsettings = new QGSettings(id);
    mUpower = new UPowerInfo;
    initBatteryInfo();
    connect(mPowerManagementGsettings,&QGSettings::changed,this,[=]{
        readSettings();
    });
    iface = new QDBusInterface(UPOWER_SERVICE,UPOWER_DISPLAY_PATH,
                              FREEDESKTOP_UPOWER,
                              QDBusConnection::systemBus());
    QDBusConnection::systemBus().connect(UPOWER_SERVICE,UPOWER_DISPLAY_PATH,
                                          FREEDESKTOP_UPOWER,
                                          "PropertiesChanged",
                                          this,
                                          SLOT(dealMessage(void))
                                          );
}

BatteryInfo::~BatteryInfo()
{
    delete iface;
    delete mUpower;
    delete mPowerManagementGsettings;
}

void BatteryInfo::initBatteryInfo()
{
    readSettings();
//    getBatteryInfo();
    mLowBatteryState = false;
}

void BatteryInfo::dealMessage()
{
    readSettings();
    getBatteryInfo();
    dealLowBatteryMessage(batteryState);
    dealBatteryIconMessage(batteryState,mPercentage);
}

void BatteryInfo::readSettings()
{
    mSetPercentageLow = mPowerManagementGsettings->get("percentageLow").toInt();
    mPercentageAction = mPowerManagementGsettings->get("percentageAction").toInt();
}

void BatteryInfo::getBatteryInfo()
{
    batteryState = mUpower->OnBattery();
    mPercentage = Percentage();
}

QString BatteryInfo::IconName()
{
//    QDBusInterface iface(UPOWER_SERVICE,UPOWER_DISPLAY_PATH,
//                          FREEDESKTOP_UPOWER,
//                          QDBusConnection::systemBus());
//     QDBusReply<QVariant> reply = iface->call("Get",UPOWER_DIVICES_SERVICE,"IconName");
//     if(reply.isValid()){
//         return reply.value().toString();
//    }else{
//         return "false";
//     }
    getBatteryInfo();
    double num = mPercentage;
    if(true == batteryState){
        mIconMessage = QString("battery-level-%1-symbolic").arg((int)num/10*10);
    }else{
        mIconMessage = QString("battery-level-%1-charging-symbolic").arg((int)num/10*10);
    }
    return mIconMessage;
}

double BatteryInfo::Percentage()
{
    double value;
//    QDBusInterface iface(UPOWER_SERVICE,UPOWER_DISPLAY_PATH,
//                          FREEDESKTOP_UPOWER,
//                          QDBusConnection::systemBus());
     QDBusReply<QVariant> reply = iface->call("Get",UPOWER_DIVICES_SERVICE,"Percentage");
     if(reply.isValid()){
         value = reply.value().toDouble();
         return value;
    }else{
         return -1.0;
     }

}

int BatteryInfo::TimeToFull()
{
//    QDBusInterface iface(UPOWER_SERVICE,UPOWER_DISPLAY_PATH,
//                          FREEDESKTOP_UPOWER,
//                          QDBusConnection::systemBus());
     QDBusReply<QVariant> reply = iface->call("Get",UPOWER_DIVICES_SERVICE,"TimeToFull");
     if(reply.isValid()){
         return reply.value().toInt();
    }else{
         return -1;
     }
}

int BatteryInfo::TimeToEmpty()
{
    QDBusInterface iface(UPOWER_SERVICE,UPOWER_DISPLAY_PATH,
                          FREEDESKTOP_UPOWER,
                          QDBusConnection::systemBus());
     QDBusReply<QVariant> reply = iface.call("Get",UPOWER_DIVICES_SERVICE,"TimeToEmpty");
     if(reply.isValid()){
         return reply.value().toInt();
    }else{
         return -1;
     }
}

bool BatteryInfo::IsPresent()
{
//    QDBusInterface iface(UPOWER_SERVICE,UPOWER_DISPLAY_PATH,
//                          FREEDESKTOP_UPOWER,
//                          QDBusConnection::systemBus());
     QDBusReply<QVariant> reply = iface->call("Get",UPOWER_DIVICES_SERVICE,"IsPresent");
     if(reply.isValid()){
         return reply.value().toInt();
    }else{
         return false;
     }
}

bool BatteryInfo::LowBatteryState()
{
    readSettings();
    getBatteryInfo();
    if (mSetPercentageLow > mPercentage) {
        return true;
    } else {
        return false;
    }
}

void BatteryInfo::dealLowBatteryMessage(const bool &state)
{
    if (state) {
        if (mSetPercentageLow > mPercentage) {
            if (!mLowBatteryState) {
                mLowBatteryState = true;
                emit LowBatteryChanged(mLowBatteryState);
            }
            if (mPercentageAction > mPercentage) {
                mVeryLowBatteryState = true;
                emit VeryLowBatteryChanged(mVeryLowBatteryState);
            }
        } else {
            if (mLowBatteryState) {
                mLowBatteryState = false;
                emit LowBatteryChanged(mLowBatteryState);
            }
        }
    } else {
        if (mLowBatteryState) {
            mLowBatteryState = false;
        }
    }
 //   qDebug()<<mLowBatteryState;
}

void BatteryInfo::dealBatteryIconMessage(const bool &state, const double &precentage)
{
    double num = precentage;
    if(true == state){
        mIconMessage = QString("battery-level-%1-symbolic").arg((int)num/10*10);
    }else{
        mIconMessage = QString("battery-level-%1-charging-symbolic").arg((int)num/10*10);
    }
    emit BatteryIconChanged(mIconMessage);
}
