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
#include <QTimer>
#include <DeviceUi.h>

class DeviceForm:public QWidget {
  Q_OBJECT public:
    explicit DeviceForm(QWidget * parent = nullptr);
    ~DeviceForm();

    public Q_SLOTS:void widget_property_change();

    void slider_changed(int value);
    void slot_device_change(DEVICE * device);
    void begin_charge_animation();
  public:
     QString icon_name;
    QString statics_icon_pre;
    QString kind;
    QString percentage;
    int percent_number;
    QString predict;
//    DEVICE *m_device;
    EngineDevice *ed;
    QString path;
    void setRemain(QString remain);
    void setKind(QString state);
    void setPercent(QString perct);
    void setIcon(QString name);
    QString calculate_value(int nValue, int nTotal);
    void set_device(DEVICE * dev);
    void set_timer(UpDeviceKind kind);
    void set_charge_animation(bool flag);
    void device_adjust_battery_parameters();
    static QString device_get_ac_online();
  protected:
    void paintEvent(QPaintEvent * event);
//    void leaveEvent(QEvent *event);
//    void enterEvent(QEvent *event);
//    void mousePressEvent(QMouseEvent *event);
  private:
     DeviceUi * ui = new DeviceUi;
    QTimer *charge_animation;
     QList < QPixmap > animation_list;
    int charging_index;
    DEV mDev;
};

#endif				// DEVICE_FORM_H
