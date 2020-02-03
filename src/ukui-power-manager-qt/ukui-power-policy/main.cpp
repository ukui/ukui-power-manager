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
#include <QCoreApplication>
#include "powerpolicy.h"
#include "policy_adaptor.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qRegisterMetaType<dbus_demo_example_struct>("dbus_demo_example_struct");
    qDBusRegisterMetaType<dbus_demo_example_struct>();
    QDBusConnection connection = QDBusConnection::systemBus();

    if (!connection.registerService("ukui.power.policy")) {
        qDebug() << "error:" << connection.lastError().message();
        exit(-1);
    }

    PowerPolicy *power= new PowerPolicy();
    new PolicyAdaptor(power);
    connection.registerObject("/ukui/power/policy", power);
    return a.exec();
}
