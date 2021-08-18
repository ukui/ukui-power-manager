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

#ifndef POWERPOLICY_H
#define POWERPOLICY_H

#include <QObject>
#include <QList>
#include "dbus_struct.h"
#include <QDBusMessage>
#include "gsettings.h"

class PowerPolicy : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface","ukui.power.policy")
public:
    explicit PowerPolicy(QObject *parent = 0);

    int process(int option);
    int script_process(QString cmd);
    int process_shell(int option);
Q_SIGNALS:
    void onbattery_change(bool);
    void ModeChanged(int);
public Q_SLOTS:
    QVariantList  return_variantlist();
    QString           control(int opt);
    void onPropertiesSlot(QDBusMessage msg);
    void onbattery_change_slot(bool flag);
    bool change_auto_switch (bool flag);
private:
    int dbus_integer;
    QString ret;
    QVariant dbus_variant;
    bool onbattery;
    bool enable_auto = false;
    int mode;
    int desire_status;
    int desire_mode;
};

#endif // POWERPOLICY_H
