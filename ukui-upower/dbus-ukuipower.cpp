/*
 * Copyright (C) 2020 The Qt Company Ltd.
 * Copyright (C) 2021 KylinSoft Co., Ltd.
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

#include "dbus-ukuipower.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class UpowerAdaptor
 */

UpowerAdaptor::UpowerAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

UpowerAdaptor::~UpowerAdaptor()
{
    // destructor
}

QString UpowerAdaptor::CanHibernate()
{
    // handle method call org.ukui.upower.CanHibernate
    QString out0;
    QMetaObject::invokeMethod(parent(), "CanHibernate", Q_RETURN_ARG(QString, out0));
    return out0;
}

QString UpowerAdaptor::DaemonVersion()
{
    // handle method call org.ukui.upower.DaemonVersion
    QString out0;
    QMetaObject::invokeMethod(parent(), "DaemonVersion", Q_RETURN_ARG(QString, out0));
    return out0;
}

QString UpowerAdaptor::IconName()
{
    // handle method call org.ukui.upower.IconName
    QString out0;
    QMetaObject::invokeMethod(parent(), "IconName", Q_RETURN_ARG(QString, out0));
    return out0;
}

bool UpowerAdaptor::IsPresent()
{
    // handle method call org.ukui.upower.IsPresent
    bool out0;
    QMetaObject::invokeMethod(parent(), "IsPresent", Q_RETURN_ARG(bool, out0));
    return out0;
}

bool UpowerAdaptor::LowBattery()
{
    // handle method call org.ukui.upower.LowBattery
    bool out0;
    QMetaObject::invokeMethod(parent(), "LowBattery", Q_RETURN_ARG(bool, out0));
    return out0;
}

QString UpowerAdaptor::MachineType()
{
    // handle method call org.ukui.upower.MachineType
    QString out0;
    QMetaObject::invokeMethod(parent(), "MachineType", Q_RETURN_ARG(QString, out0));
    return out0;
}

bool UpowerAdaptor::OnBattery()
{
    // handle method call org.ukui.upower.OnBattery
    bool out0;
    QMetaObject::invokeMethod(parent(), "OnBattery", Q_RETURN_ARG(bool, out0));
    return out0;
}

double UpowerAdaptor::Percentage()
{
    // handle method call org.ukui.upower.Percentage
    double out0;
    QMetaObject::invokeMethod(parent(), "Percentage", Q_RETURN_ARG(double, out0));
    return out0;
}

int UpowerAdaptor::State()
{
    // handle method call org.ukui.upower.State
    int out0;
    QMetaObject::invokeMethod(parent(), "State", Q_RETURN_ARG(int, out0));
    return out0;
}

int UpowerAdaptor::TimeToEmpty()
{
    // handle method call org.ukui.upower.TimeToEmpty
    int out0;
    QMetaObject::invokeMethod(parent(), "TimeToEmpty", Q_RETURN_ARG(int, out0));
    return out0;
}

int UpowerAdaptor::TimeToFull()
{
    // handle method call org.ukui.upower.TimeToFull
    int out0;
    QMetaObject::invokeMethod(parent(), "TimeToFull", Q_RETURN_ARG(int, out0));
    return out0;
}

QString UpowerAdaptor::UPowerVersion()
{
    // handle method call org.ukui.upower.UPowerVersion
    QString out0;
    QMetaObject::invokeMethod(parent(), "UPowerVersion", Q_RETURN_ARG(QString, out0));
    return out0;
}

