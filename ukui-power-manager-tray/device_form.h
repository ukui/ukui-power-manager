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
#ifndef DEVICE_FORM_H
#define DEVICE_FORM_H

#include <QWidget>
#include "device.h"
#include "enginedevice.h"
#include <QMovie>

namespace Ui {
class DeviceForm;
}

class DeviceForm : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceForm(QWidget *parent = nullptr);
    ~DeviceForm();

public Q_SLOTS:
    void widget_property_change();

    void slider_changed(int value);
    void slot_device_change(DEVICE *device);
public:
    QMovie *mov;
    QString icon_name;
    QString state_text;
    QString percentage;
    int percent_number;
    QString predict;
    DEVICE *m_device;
    EngineDevice *ed;
    QString path;
    void setRemain(QString remain);
    void setState(QString state);
    void setPercent(QString perct);
    void setIcon(QString name);
    QString calculate_value(int nValue, int nTotal);
    void set_device(DEVICE *dev);
protected:
    void leaveEvent(QEvent *event);
    void enterEvent(QEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
private:
    Ui::DeviceForm *ui;
};

#endif // DEVICE_FORM_H
