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

#include "statistics-common.h"
#include "QDBusMessage"

class DEVICE : public QObject
{
    Q_OBJECT
public:
    explicit DEVICE(QObject *parent = nullptr);
    ~DEVICE();
Q_SIGNALS:
    void device_property_changed(QString path);


public Q_SLOTS:
    void handleChanged(QDBusMessage msg);
public:
    DEV *m_dev;
};

#endif // DEVICE_H
