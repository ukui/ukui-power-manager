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

#ifndef DBUS-UKUIPOWER_H
#define DBUS-UKUIPOWER_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "dbus.h"
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface org.ukui.upower
 */
class UpowerAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.ukui.upower")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.ukui.upower\">\n"
"    <method name=\"UPowerVersion\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"    </method>\n"
"    <method name=\"CanHibernate\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"    </method>\n"
"    <method name=\"MachineType\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"    </method>\n"
"    <method name=\"OnBattery\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"    </method>\n"
"    <method name=\"DaemonVersion\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"    </method>\n"
"    <method name=\"IconName\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"    </method>\n"
"    <method name=\"Percentage\">\n"
"      <arg direction=\"out\" type=\"d\"/>\n"
"    </method>\n"
"    <method name=\"TimeToFull\">\n"
"      <arg direction=\"out\" type=\"i\"/>\n"
"    </method>\n"
"    <method name=\"TimeToEmpty\">\n"
"      <arg direction=\"out\" type=\"i\"/>\n"
"    </method>\n"
"    <method name=\"IsPresent\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"    </method>\n"
"    <method name=\"State\">\n"
"      <arg direction=\"out\" type=\"i\"/>\n"
"    </method>\n"
"    <method name=\"LowBattery\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    UpowerAdaptor(QObject *parent);
    virtual ~UpowerAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    QString CanHibernate();
    QString DaemonVersion();
    QString IconName();
    bool IsPresent();
    bool LowBattery();
    QString MachineType();
    bool OnBattery();
    double Percentage();
    int State();
    int TimeToEmpty();
    int TimeToFull();
    QString UPowerVersion();
Q_SIGNALS: // SIGNALS
};

#endif
