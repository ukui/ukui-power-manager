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

#ifndef DEVICE_H
#define DEVICE_H

#include <QDBusMessage>
#include "engine_common.h"

struct DEV
{
    UpDeviceKind kind;
    UpDeviceLevel warnlevel;
    QString Device;
    QString Type;
    QString PowerSupply;
    QString Online;
    QString Model;
    QString Energy;
    QString EnergyEmpty;
    QString EnergyFull;
    QString EnergyRate;
    bool IsPresent;
    QString IsRechargeable;
    double Percentage;
    UpDeviceState State;
    qlonglong TimeToEmpty;
    qlonglong TimeToFull;
    QString Voltage;
    double Capacity;
    QString Technology;
    QString path;

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
