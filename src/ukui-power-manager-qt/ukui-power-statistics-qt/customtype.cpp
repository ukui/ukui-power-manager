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
#include "customtype.h"
#include <libintl.h>

void registerCustomType()
{
    qDBusRegisterMetaType<StructUdu>();
    qDBusRegisterMetaType<QList<StructUdu>>();

}


QDBusArgument &operator<<(QDBusArgument &argument, const StructUdu &structudp)
{
    argument.beginStructure();
    argument<<structudp.time<<structudp.value<<structudp.state;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, StructUdu &structudp)
{
    argument.beginStructure();
    argument>>structudp.time>>structudp.value>>structudp.state;
    argument.endStructure();
    return argument;
}


QDBusArgument &operator<<(QDBusArgument &argument, const QList<StructUdu> &myarray)
{
    argument.beginArray(qMetaTypeId<StructUdu>());
    for(int i= 0; i<myarray.length(); i++)
        argument << myarray.at(i);
    argument.endArray();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QList<StructUdu> &myarray)
{
    argument.beginArray();
    myarray.clear();
    while(!argument.atEnd())
    {
        StructUdu element;
        argument>>element;
        myarray.append(element);
    }
    argument.endArray();
    return argument;
}

