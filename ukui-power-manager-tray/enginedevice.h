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
#ifndef ENGINEDEVICE_H
#define ENGINEDEVICE_H

#include <QObject>
#include <device.h>
#include "customtype.h"
#include <QString>
#include <QGSettings>


#define DBUS_SERVICE "org.freedesktop.UPower"
#define DBUS_OBJECT "/org/freedesktop/UPower"
#define DBUS_INTERFACE "org.freedesktop.UPower"

#define DBUS_INTERFACE_PRO "org.freedesktop.DBus.Properties"
#define DBUS_INTERFACE_DEV "org.freedesktop.UPower.Device"
class EngineDevice : public QObject
{
    Q_OBJECT
private:
    static EngineDevice* instance;
    explicit EngineDevice(QObject *parent = nullptr);
    class  Deconstructor
    {
    public:
         ~Deconstructor() {
            if(instance)
            {
                delete instance;
                instance = nullptr;
            }
        }
    };
    static Deconstructor deconstructor;

public:
    static EngineDevice* getInstance()
    {
        if(instance==nullptr)
        {
            instance = new EngineDevice;

        }
        return instance;
    }

Q_SIGNALS:
    void icon_changed(QString);
    void engine_signal_discharge(DEV dv);
    void engine_signal_fullycharge(DEV dv);
    void engine_signal_charge_low(DEV dv);
    void engine_signal_charge_critical(DEV dv);
    void engine_signal_charge_action(DEV dv);
    void engine_signal_summary_change(QString summary);

public Q_SLOTS:
    void power_device_change_callback(QDBusMessage msg, QString path);

    void engine_policy_settings_cb();
public:
    QGSettings *settings;
    QList<DEVICE*> devices;
    QString previous_icon;
    QString privious_sum;
    QString previous_summary;

    bool use_time_primary;
    bool time_is_accurate;

    uint			 low_percentage;
    uint			 critical_percentage;
    uint			 action_percentage;
    uint			 low_time;
    uint			 critical_time;
    uint			 action_time;
    DEVICE *composite_device;
    GpmIconPolicy icon_policy;
    bool power_device_recalculate_icon();
    void power_device_recalculate_state();
    QString power_device_get_icon_exact(UpDeviceKind device_kind, UpDeviceLevel warning, bool use_state);
    bool gpm_engine_recalculate_summary();
    bool engine_recalculate_summary();
    void power_device_cold_plug();
    void getProperty(QString path, DEV &dev);
    QString engine_get_summary();
    QString engine_kind_to_string(UpDeviceKind type_enum);
    QString engine_kind_to_localised_text(UpDeviceKind kind, uint number);
    void power_device_get_devices();
    void power_device_add(QDBusMessage msg);
    void power_device_remove(QDBusMessage msg);
    QString boolToString(bool ret);
    QString engine_get_device_summary(DEVICE *dv);
    QString engine_get_device_icon_index(qreal percentage);
    QString power_device_get_icon();
    QString engine_get_device_icon(DEVICE *device);
    void putAttributes(QMap<QString, QVariant> &map, DEV &btrDetailData);
    QString engine_get_state_text(UpDeviceState state);
    QString engine_get_device_predict(DEVICE *dv);
    QString gpm_get_timestring(int time_secs);
};

#endif // ENGINEDEVICE_H
