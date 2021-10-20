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

#include "dbus.h"

#define FREEDESKTOP_UPOWER                  "org.freedesktop.DBus.Properties"
#define UPOWER_PATH                         "/org/freedesktop/UPower"
#define UPOWER_SERVICE                      "org.freedesktop.UPower"
#define UPOWER_DISPLAY_PATH                 "/org/freedesktop/UPower/devices/DisplayDevice"


DBus::DBus(QObject *parent) : QObject(parent)
{
    upowerinfo = new UPowerInfo;
    batteryinfo = new BatteryInfo;
    connect(upowerinfo,&UPowerInfo::acChanged,this,&DBus::dealPowerState);
    connect(upowerinfo,&UPowerInfo::lidChanged,this,&DBus::dealLidState);
    connect(batteryinfo,&BatteryInfo::LowBatteryChanged,this,&DBus::dealLowBattery);
    connect(batteryinfo,&BatteryInfo::VeryLowBatteryChanged,this,&DBus::dealVeryLowBattery);
    connect(batteryinfo,&BatteryInfo::BatteryIconChanged,this,&DBus::dealBatteryIcon);
}

DBus::~DBus()
{
    delete upowerinfo;
    delete batteryinfo;
}

QString DBus::UPowerVersion()
{
    return "3.1";
}

QString DBus::CanHibernate()
{
    return upowerinfo->CanHibernate();
}

QString DBus::MachineType()
{
    return upowerinfo->MachineType();
}

bool DBus::OnBattery()
{
    return upowerinfo->OnBattery();
}

QString DBus::DaemonVersion()
{
    return upowerinfo->DaemonVersion();
}



//*****************************************

/******************************************/


QString DBus::IconName()
{
    return batteryinfo->IconName();
}

double DBus::Percentage()
{
    return batteryinfo->Percentage();
}

int DBus::TimeToFull()
{
    return batteryinfo->TimeToFull();
}

int DBus::TimeToEmpty()
{
    return batteryinfo->TimeToEmpty();
}

bool DBus::IsPresent()
{
    return batteryinfo->IsPresent();
}

int DBus::State()
{
    int i=1;
    return i;
}

bool DBus::LowBattery()
{
    return batteryinfo->LowBatteryState();
}

void DBus::dealPowerState(bool state)
{
    emit PowerState(state);
}

void DBus::dealLidState(bool state)
{
    emit LidState(state);
}

void DBus::dealLowBattery(bool state)
{
    emit LowBatteryState(state);
}

void DBus::dealVeryLowBattery(bool state)
{
    emit VeryLowBatteryState(state);
}

void DBus::dealBatteryIcon(QString msg)
{
    emit BatteryIcon(msg);
}
