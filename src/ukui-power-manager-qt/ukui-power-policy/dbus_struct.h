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
#include <QDBusArgument>
#include <QString>

#ifndef DBUS_DEMO_EXAMPLE_STRUCT
#define	DBUS_DEMO_EXAMPLE_STRUCT
struct dbus_demo_example_struct
{
    int drv_ID;
    QString name;
    QString full_name;
    int notify_mid;

    friend QDBusArgument &operator<<(QDBusArgument &argument, const dbus_demo_example_struct&mystruct)
    {
        argument.beginStructure();
        argument << mystruct.drv_ID << mystruct.name << mystruct.full_name << mystruct.notify_mid;
        argument.endStructure();
        return argument;
    }

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, dbus_demo_example_struct&mystruct)
    {
        argument.beginStructure();
        argument >> mystruct.drv_ID >> mystruct.name >> mystruct.full_name >> mystruct.notify_mid;
        argument.endStructure();
        return argument;
    }

};

Q_DECLARE_METATYPE(dbus_demo_example_struct)
#endif
