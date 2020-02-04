/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
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
#ifndef DEVICE_H
#define DEVICE_H

#include <QDBusMessage>
#include "engine-common.h"

struct DEV
{
    UpDeviceKind kind;
    UpDeviceLevel warnlevel;
    QString Device;
    QString Type;
    QString PowerSupply;
    QString Online;
    QString Vendor;
    QString Model;
    QString Refresh;
    QString Energy;
    QString EnergyEmpty;
    QString EnergyFull;
    QString EnergyFullDesign;
    QString EnergyRate;
    bool IsPresent;
    QString IsRechargeable;
    double Percentage;
    UpDeviceState State;
    qlonglong TimeToEmpty;
    qlonglong TimeToFull;
    QString Voltage;
    double Capacity;
    QString update_time;
    QString serial;
    QString Technology;
    bool hasHistory;
    bool hasStat;
    QString path;

    bool autorange_x;
    int start_x;
    int stop_x;
    bool autorange_y;
    int start_y;
    int stop_y;
};

class DEVICE : public QObject
{
    Q_OBJECT
public:
    explicit DEVICE(QObject *parent = nullptr);
    ~DEVICE();
Q_SIGNALS:
    void device_property_changed(QDBusMessage msg,QString path);


public Q_SLOTS:
    void handleChanged(QDBusMessage msg);
public:
    DEV m_dev;
};

#endif // DEVICE_H
