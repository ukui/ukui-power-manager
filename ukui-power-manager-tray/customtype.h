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
#ifndef CUSTOMTYPE_H
#define CUSTOMTYPE_H

#include <QDBusMetaType>
#include <QDBusVariant>
/**
 * QDBusMetaType:
 *
 * register qdbus type.
 **/
struct StructUdu
{
    quint32 time;
    qreal value;
    quint32 state;
};

QDBusArgument &operator<<(QDBusArgument &argument, const StructUdu &structudp);
const QDBusArgument &operator>>(const QDBusArgument &argument, StructUdu &structudp);
QDBusArgument &operator<<(QDBusArgument &argument, const QList<StructUdu> &myarray);
const QDBusArgument &operator>>(const QDBusArgument &argument, QList<StructUdu> &myarray);

void registerCustomType();


Q_DECLARE_METATYPE(QList<StructUdu>)
Q_DECLARE_METATYPE(StructUdu)


#define GPM_UP_TIME_PRECISION			5*60
#define GPM_UP_TEXT_MIN_TIME			120


int precision_round_down (float value, int smallest);
int precision_round_up (float value, int smallest);


#endif // CUSTOMTYPE_H
