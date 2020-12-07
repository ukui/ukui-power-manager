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
#include "enginedevice.h"
#include "device.h"
#include <QDBusObjectPath>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDebug>

#include "engine_common.h"

EngineDevice* EngineDevice::instance = NULL;

void EngineDevice::power_device_get_devices()
{

    QList<QDBusObjectPath> deviceNames;

    /* call enum dbus*/
    QDBusMessage msg = QDBusMessage::createMethodCall(DBUS_SERVICE,DBUS_OBJECT,
            DBUS_INTERFACE,"EnumerateDevices");
    QDBusMessage res = QDBusConnection::systemBus().call(msg);

    if(res.type() == QDBusMessage::ReplyMessage)
    {
        const QDBusArgument &dbusArg = res.arguments().at(0).value<QDBusArgument>();
        dbusArg >> deviceNames;
    }
    else {
    }
    int len = deviceNames.size();
    for(int i = 0; i < len; i++)
    {
        DEVICE *device = new DEVICE;
        device->m_dev.path = deviceNames.at(i).path();
        getProperty(device->m_dev.path,device->m_dev);
        /* connect the nofity signal to changecallback */
        QDBusConnection::systemBus().connect(DBUS_SERVICE,device->m_dev.path,DBUS_INTERFACE_PRO,
                                             QString("PropertiesChanged"),device,SLOT(handleChanged(QDBusMessage)));
        connect(device,SIGNAL(device_property_changed(QDBusMessage,QString)),this,SLOT(power_device_change_callback(QDBusMessage,QString)));

        /* add to device list*/
        devices.append(device);
    }

}


EngineDevice::EngineDevice(QObject *parent) : QObject(parent)
{
    icon_policy = GPM_ICON_POLICY_PRESENT;

    power_device_get_devices();
    QDBusConnection::systemBus().connect(DBUS_SERVICE,DBUS_OBJECT,DBUS_SERVICE,
                                         ("DeviceAdded"),this,SLOT(power_device_add(QDBusObjectPath)));
    QDBusConnection::systemBus().connect(DBUS_SERVICE,DBUS_OBJECT,DBUS_SERVICE,
                                         ("DeviceRemoved"),this,SLOT(power_device_remove(QDBusObjectPath)));
    settings = new QGSettings(GPM_SETTINGS_SCHEMA);
    connect(settings,SIGNAL(changed(const QString&)),this,SLOT(engine_policy_settings_cb(const QString&)));

}

void EngineDevice::engine_policy_settings_cb(const QString& str)
{
    if(str != "iconPolicy")
        return;
    QVariant var =settings->get(GPM_SETTINGS_KEY_POLICY);
    QString icon_policy_str = var.value<QString>();
    if(icon_policy_str == "always")
        icon_policy = GPM_ICON_POLICY_ALWAYS;
    else if(icon_policy_str == "present")
        icon_policy = GPM_ICON_POLICY_PRESENT;
    else if(icon_policy_str == "charge")
        icon_policy = GPM_ICON_POLICY_CHARGE;
    else if(icon_policy_str == "low")
        icon_policy = GPM_ICON_POLICY_LOW;
    else if(icon_policy_str == "critical")
        icon_policy = GPM_ICON_POLICY_CRITICAL;
    else if(icon_policy_str == "never")
        icon_policy = GPM_ICON_POLICY_NEVER;

    power_device_recalculate_icon();
}

void EngineDevice::power_device_add(QDBusObjectPath msg)
{
    /* assign warning */
    /* check capacity */
    /* get device properties */
    QDBusMessage msgTmp = QDBusMessage::createMethodCall(DBUS_SERVICE,msg.path(),
            "org.freedesktop.DBus.Properties","GetAll");
    msgTmp << DBUS_INTERFACE_DEV;
    QDBusMessage res = QDBusConnection::systemBus().call(msgTmp);
    if(res.type() == QDBusMessage::ReplyMessage)
    {
        const QDBusArgument &dbusArg = res.arguments().at(0).value<QDBusArgument>();
        QMap<QString,QVariant> map;
        dbusArg >> map;
        UpDeviceKind kind = (UpDeviceKind)map.value("Type").toInt();

        if(kind != UP_DEVICE_KIND_LINE_POWER && kind != UP_DEVICE_KIND_BATTERY && kind != UP_DEVICE_KIND_PHONE && kind != UP_DEVICE_KIND_COMPUTER)
            return;

        DEVICE *dev = new DEVICE;
        dev->m_dev.path = msg.path();
        dev->m_dev.kind = kind;
        dev->m_dev.warnlevel = (UpDeviceLevel)map.value("WarningLevel").toUInt();

        dev->m_dev.Type = engine_kind_to_localised_text ((UpDeviceKind)map.value(QString("Type")).toInt(),1);
        dev->m_dev.Model = map.value(QString("Model")).toString();
        dev->m_dev.Device = map.value(QString("NativePath")).toString();
        dev->m_dev.Capacity = (map.value(QString("Capacity")).toDouble());
        dev->m_dev.Energy = QString::number(map.value(QString("Energy")).toDouble(), 'f', 1)+ " Wh";
        dev->m_dev.EnergyEmpty= QString::number(map.value(QString("EnergyEmpty")).toDouble(), 'f', 1)+ " Wh";
        dev->m_dev.EnergyFull = QString::number(map.value(QString("EnergyFull")).toDouble(), 'f', 1)+ " Wh";
        dev->m_dev.EnergyRate = QString::number(map.value(QString("EnergyRate")).toDouble(), 'f', 1) + " W";

        dev->m_dev.IsPresent = (map.value(QString("IsPresent")).toBool());

        dev->m_dev.PowerSupply = boolToString(map.value(QString("PowerSupply")).toBool());
        dev->m_dev.Percentage = map.value(QString("Percentage")).toDouble();
        dev->m_dev.Percentage = ( (float)( (int)( (dev->m_dev.Percentage + 0.05) * 10 ) ) ) / 10;
        dev->m_dev.Online = boolToString(map.value(QString("Online")).toBool());
        dev->m_dev.State = (UpDeviceState)map.value(QString("State")).toInt();
        dev->m_dev.TimeToEmpty = map.value(QString("TimeToEmpty")).toLongLong();
        dev->m_dev.TimeToFull = map.value(QString("TimeToFull")).toLongLong();

        /*add to array*/
        devices.append(dev);
        QDBusConnection::systemBus().connect(DBUS_SERVICE,dev->m_dev.path,DBUS_INTERFACE_PRO,
                                             QString("PropertiesChanged"),dev,SLOT(handleChanged(QDBusMessage)));
        /*connect notify signals*/
        connect(dev,SIGNAL(device_property_changed(QDBusMessage,QString)),this,SLOT(power_device_change_callback(QDBusMessage,QString)));
        /*recaculate state*/
        Q_EMIT one_device_add(dev);

    }
}

void EngineDevice::power_device_remove(QDBusObjectPath msg)
{
    Q_FOREACH (auto item, devices)
    {
        if(item->m_dev.path == msg.path())
        {
            devices.removeOne(item);
            Q_EMIT one_device_remove(item);
            break;
        }
    }

}


void EngineDevice::getProperty(QString path,DEV& dev)
{

    QDBusMessage msg = QDBusMessage::createMethodCall(DBUS_SERVICE,path,
            DBUS_INTERFACE_PRO,"GetAll");
    msg << DBUS_INTERFACE_DEV;
    QDBusMessage res = QDBusConnection::systemBus().call(msg);

    if(res.type() == QDBusMessage::ReplyMessage)
    {
        const QDBusArgument &dbusArg = res.arguments().at(0).value<QDBusArgument>();
        QMap<QString,QVariant> map;
        dbusArg >> map;

        dev.kind = (UpDeviceKind)map.value(QString("Type")).toInt();
        dev.Type = engine_kind_to_localised_text ((UpDeviceKind)map.value(QString("Type")).toInt(),1);
        dev.Model = map.value(QString("Model")).toString();
        dev.Device = map.value(QString("NativePath")).toString();

        dev.Capacity = (map.value(QString("Capacity")).toDouble());
        dev.Energy = QString::number(map.value(QString("Energy")).toDouble(), 'f', 1)+ " Wh";
        dev.EnergyEmpty= QString::number(map.value(QString("EnergyEmpty")).toDouble(), 'f', 1)+ " Wh";
        dev.EnergyFull = QString::number(map.value(QString("EnergyFull")).toDouble(), 'f', 1)+ " Wh";
        dev.EnergyRate = QString::number(map.value(QString("EnergyRate")).toDouble(), 'f', 1) + " W";
        dev.IsPresent = (map.value(QString("IsPresent")).toBool());
        dev.PowerSupply = boolToString(map.value(QString("PowerSupply")).toBool());
        dev.Percentage = map.value(QString("Percentage")).toDouble();
        dev.Percentage = ( (float)( (int)( (dev.Percentage + 0.05) * 10 ) ) ) / 10;

        dev.Online = boolToString(map.value(QString("Online")).toBool());

        dev.State = (UpDeviceState)map.value(QString("State")).toInt();

        dev.TimeToEmpty = map.value(QString("TimeToEmpty")).toLongLong();
        dev.TimeToFull = map.value(QString("TimeToFull")).toLongLong();
        dev.Voltage = QString::number(map.value(QString("Voltage")).toDouble(), 'f', 1) + " V";

    }
}

QString EngineDevice::boolToString(bool ret)
{
    return ret ? tr("yes") : tr("no");
}


void EngineDevice::putAttributes(QMap<QString,QVariant>& map,DEV &btrDetailData)
{
    if(map.contains("TimeToFull"))
    {
        btrDetailData.TimeToFull= map.value(QString("TimeToFull")).toLongLong();
    }
    if(map.contains("TimeToEmpty"))
        btrDetailData.TimeToEmpty = map.value(QString("TimeToEmpty")).toLongLong();
    if(map.contains("EnergyRate"))
        btrDetailData.EnergyRate = QString::number(map.value(QString("EnergyRate")).toDouble(), 'f', 1) + " W";
    if(map.contains("Energy"))
        btrDetailData.Energy = QString::number(map.value(QString("Energy")).toDouble(), 'f', 1)+ " Wh";
    if(map.contains("Voltage"))
        btrDetailData.Voltage = QString::number(map.value(QString("Voltage")).toDouble(), 'f', 1) + " V";
    if(map.contains("WarnLevel"))
    {
        btrDetailData.warnlevel = (UpDeviceLevel)map.value(QString("WarnLevel")).toInt();
    }

    if(map.contains("State"))
    {
        btrDetailData.State = (UpDeviceState)map.value(QString("State")).toInt();
        //qDebug()<< "state:" << btrDetailData.State;
    }
    if(map.contains("Percentage"))
    {
        btrDetailData.Percentage = map.value(QString("Percentage")).toDouble();
        btrDetailData.Percentage = ( (float)( (int)( (btrDetailData.Percentage + 0.05) * 10 ) ) ) / 10;
    }

    if(map.contains("PowerSupply"))
        btrDetailData.PowerSupply = (map.value(QString("PowerSupply")).toBool()) ? tr("Yes") :tr("No");

    if(map.contains("IsPresent"))
        btrDetailData.IsPresent = (map.value(QString("IsPresent")).toBool());
}

void EngineDevice::power_device_change_callback(QDBusMessage msg,QString path)
{
    /* if battery change to display devices */
    /* judge state */

    DEVICE *item = nullptr;
    Q_FOREACH (auto item_tmp, devices)
    {
        if(item_tmp->m_dev.path == path)
        {
            item = item_tmp;
            break;
        }
    }

    if(item == nullptr)
        return;
    DEV tmp_dev = item->m_dev;
    const QDBusArgument &arg = msg.arguments().at(1).value<QDBusArgument>();
    QMap<QString,QVariant> map;
    arg >> map;
    putAttributes(map,item->m_dev);
    Q_EMIT signal_device_change(item);
    if(item->m_dev.State != tmp_dev.State)
    {
        if(item->m_dev.State==UP_DEVICE_STATE_DISCHARGING)
        {
            Q_EMIT engine_signal_discharge(tmp_dev);
        }
        else if (item->m_dev.State==UP_DEVICE_STATE_FULLY_CHARGED)
        {
            Q_EMIT engine_signal_fullycharge(tmp_dev);
        }
        //save new state
    }
    /* check the warning state has not changed */
    if(item->m_dev.warnlevel != tmp_dev.warnlevel)
    {
        if(item->m_dev.warnlevel==UP_DEVICE_LEVEL_LOW)
        {
            Q_EMIT engine_signal_charge_low(tmp_dev);
        }
        else if (item->m_dev.warnlevel==UP_DEVICE_LEVEL_CRITICAL)
        {
            Q_EMIT engine_signal_charge_critical(tmp_dev);
        }
        else if(item->m_dev.warnlevel==UP_DEVICE_LEVEL_ACTION)
        {
            Q_EMIT engine_signal_charge_action(tmp_dev);
        }
        //save new warning;
    }

    /*recaculate state*/
    power_device_recalculate_state();
}

void EngineDevice::power_device_recalculate_state()
{

    /*recaculate icon*/
    power_device_recalculate_icon();
    /*recaculate sum*/
    engine_recalculate_summary();
}

bool EngineDevice::power_device_recalculate_icon()
{
    /*call a function to obtain icon*/
    QString icon;
    icon = power_device_get_icon();
    if(icon.isNull())
    {
        if(previous_icon.isNull())
            return false;
        Q_EMIT icon_changed(QString());
        previous_icon.clear();
        return true;
    }
    if(previous_icon.isNull())
    {
        Q_EMIT icon_changed(icon);
        previous_icon = icon;
        return true;
    }

    if(previous_icon != icon)
    {
        Q_EMIT icon_changed(icon);
        previous_icon = icon;
        return true;
    }
    return false;
}

/**
 * power_device_get_icon:
 *
 * Returns the icon
 **/
QString EngineDevice::power_device_get_icon()
{
    QString icon;

    /* GPM_ICON_POLICY_NEVER */
    if (icon_policy == GPM_ICON_POLICY_NEVER) {
        return QString();
    }

    /* we try CRITICAL: BATTERY, UPS, MOUSE, KEYBOARD */
    if (icon_policy == GPM_ICON_POLICY_CRITICAL || icon_policy == GPM_ICON_POLICY_ALWAYS)
    {
        icon = power_device_get_icon_exact (UP_DEVICE_KIND_BATTERY, UP_DEVICE_LEVEL_CRITICAL, false);
        if (!icon.isNull()) {
            return icon;
        }
        icon = power_device_get_icon_exact (UP_DEVICE_KIND_UPS, UP_DEVICE_LEVEL_CRITICAL, false);
        if (!icon.isNull()) {
            return icon;
        }
        icon = power_device_get_icon_exact (UP_DEVICE_KIND_MOUSE, UP_DEVICE_LEVEL_CRITICAL, false);
        if (!icon.isNull()) {
            return icon;
        }
        icon = power_device_get_icon_exact (UP_DEVICE_KIND_KEYBOARD, UP_DEVICE_LEVEL_CRITICAL, false);
        if (!icon.isNull()) {
            return icon;
        }
        if(icon_policy == GPM_ICON_POLICY_CRITICAL)
        {
            return QString();
        }
    }


    /* we try GPM_ENGINE_WARNING_LOW: BATTERY, UPS, MOUSE, KEYBOARD */
    if (icon_policy == GPM_ICON_POLICY_LOW || icon_policy == GPM_ICON_POLICY_ALWAYS)
    {
        icon = power_device_get_icon_exact (UP_DEVICE_KIND_BATTERY, UP_DEVICE_LEVEL_LOW, false);
        if (!icon.isNull()) {
            return icon;
        }
        icon = power_device_get_icon_exact (UP_DEVICE_KIND_UPS, UP_DEVICE_LEVEL_LOW, false);
        if (!icon.isNull()) {
            return icon;
        }
        icon = power_device_get_icon_exact (UP_DEVICE_KIND_MOUSE, UP_DEVICE_LEVEL_LOW, false);
        if (!icon.isNull()) {
            return icon;
        }
        icon = power_device_get_icon_exact (UP_DEVICE_KIND_KEYBOARD, UP_DEVICE_LEVEL_LOW, false);
        if (!icon.isNull()) {
            return icon;
        }
        if(icon_policy == GPM_ICON_POLICY_LOW)
        {
            return QString();
        }
    }

    /* we try (DIS)CHARGING: BATTERY, UPS */
    if (icon_policy == GPM_ICON_POLICY_CHARGE || icon_policy == GPM_ICON_POLICY_ALWAYS)
    {
        icon = power_device_get_icon_exact (UP_DEVICE_KIND_BATTERY, UP_DEVICE_LEVEL_NONE, true);
        if (!icon.isNull()) {
            return icon;
        }

        icon = power_device_get_icon_exact (UP_DEVICE_KIND_UPS, UP_DEVICE_LEVEL_NONE, true);
        if (!icon.isNull()) {
            return icon;
        }
        if(icon_policy == GPM_ICON_POLICY_CHARGE)
        {
            return QString();
        }
    }

    /* we try PRESENT: BATTERY, UPS */
    if (icon_policy == GPM_ICON_POLICY_PRESENT || icon_policy == GPM_ICON_POLICY_ALWAYS)
    {
        icon = power_device_get_icon_exact (UP_DEVICE_KIND_BATTERY, UP_DEVICE_LEVEL_NONE, false);
        if (!icon.isNull()) {
            return icon;
        }
        icon = power_device_get_icon_exact (UP_DEVICE_KIND_UPS, UP_DEVICE_LEVEL_NONE, false);
        if (!icon.isNull()) {
            return icon;
        }
        if(icon_policy == GPM_ICON_POLICY_PRESENT)
        {
            return QString();
        }
    }

    //always
    icon = power_device_get_icon_exact (UP_DEVICE_KIND_BATTERY, UP_DEVICE_LEVEL_NONE, false);
    if (!icon.isNull()) {
        return icon;
    }
    /* we fallback to the ac_adapter icon */
    return QString("battery-full-symbolic");
}
/**
 * power_device_get_icon_exact:
 *
 * Returns the icon
 **/
QString EngineDevice::power_device_get_icon_exact (UpDeviceKind device_kind, UpDeviceLevel warning, bool use_state)
{
    DEVICE *device;
    UpDeviceLevel warning_temp;
    UpDeviceState state;
    /* do we have specific device types? */
    Q_FOREACH (device, devices) {

        state = device->m_dev.State;
        warning_temp = device->m_dev.warnlevel;
        if ((device->m_dev.kind == device_kind) && (device->m_dev.IsPresent)) {
            if (warning != UP_DEVICE_LEVEL_NONE) {
                if (warning_temp == warning) {
                    composite_device = device;
                    return engine_get_device_icon (device);
                }
                continue;
            }
            if (use_state) {
//                if (state == UP_DEVICE_STATE_CHARGING || state == UP_DEVICE_STATE_DISCHARGING) {
                if ( state == UP_DEVICE_STATE_DISCHARGING) {
                    composite_device = device;
                    return engine_get_device_icon (device);
                }

                continue;
            }
            composite_device = device;
            return engine_get_device_icon (device);
        }
    }
    return QString();
}

/**
 * engine_recalculate_summary:
 */
bool EngineDevice::engine_recalculate_summary ()
{
    QString summary;

    summary = engine_get_summary ();
    if (previous_summary.isNull()) {
        previous_summary = summary;
        Q_EMIT engine_signal_summary_change(summary);
        return true;
    }

    if (previous_summary != summary) {
        previous_summary = summary;
        Q_EMIT engine_signal_summary_change(summary);
        return true;
    }
    return false;
}

/**
 * engine_get_summary:
 *
 * Returns the complete tooltip ready for display
 **/
QString EngineDevice::engine_get_summary ()
{
    DEVICE *device;
    UpDeviceState state;
    QString tooltip;
    QString part;
    bool is_present;
    UpDeviceKind kind;

    Q_FOREACH (device, devices) {
        is_present = device->m_dev.IsPresent;
        state = device->m_dev.State;
        kind = device->m_dev.kind;
        if ((!is_present)||(kind != UP_DEVICE_KIND_BATTERY))
            continue;
        if (state == UP_DEVICE_STATE_EMPTY)
            continue;
        part = engine_get_device_summary (device);
        if (!part.isNull())
            tooltip = QString("%1").arg(part);
    }
    return tooltip;
}

/**
 *
 * Returns a localised timestring
 *
 * Return value: The time string, e.g. "2 hours 3 minutes"
 **/
QString EngineDevice::engine_get_timestring (int time_secs)
{
    int  hours;
    int  minutes;
    QString result;
    QString tmp;
    QString hour_str;
    QString minute_str;
    /* Add 0.5 to do rounding */
    minutes = (int) ( ( time_secs / 60.0 ) + 0.5 );

    if (minutes == 0) {
        result = (tr("Unknown time"));
        return result;
    }

    if (minutes < 60) {
        if(minutes==1)
            minute_str = tr("minute");
        else
            minute_str=tr("minutes");
        result=QString("%1 %2").arg(minutes).arg(minute_str);
        return result;
    }

    hours = minutes / 60;
    minutes = minutes % 60;

    if (minutes == 0)
    {
        if(hours == 1)
            hour_str = tr("hour");
        else
            hour_str = tr("hours");
        result=QString("%1 %2").arg(hours).arg(hour_str);
    }
    else
    {
        if(minutes==1)
            minute_str = tr("minute");
        else
            minute_str=tr("minutes");
        if(hours == 1)
            hour_str = tr("hour");
        else
            hour_str = tr("hours");

        result = QString("%1 %2 %3 %4").arg(hours).arg(hour_str).arg(minutes).arg(minute_str);

    }
    return result;
}


/**
 * engine_get_device_predict:
 **/
QString EngineDevice::engine_get_device_predict(DEVICE* dv)
{
    QString result;

    QString kind_desc;
    uint time_to_full_round;
    uint time_to_empty_round;
    QString time_to_full_str;
    QString time_to_empty_str;
    UpDeviceKind kind;
    UpDeviceState state;
    double percentage;
    bool is_present;
    uint time_to_full;
    uint time_to_empty;

    kind = dv->m_dev.kind;
    is_present = dv->m_dev.IsPresent;
    state = dv->m_dev.State;
    percentage = dv->m_dev.Percentage;

    time_to_empty = dv->m_dev.TimeToEmpty;
    time_to_full = dv->m_dev.TimeToFull;
    if (!is_present)
    {
        return NULL;
    }

    kind_desc = engine_kind_to_localised_text (kind, 1);

    if (kind == UP_DEVICE_KIND_MOUSE ||
        kind == UP_DEVICE_KIND_KEYBOARD ||
        kind == UP_DEVICE_KIND_PDA)
    {
        result = QString("%1(%2%)").arg(kind_desc).arg(percentage);
        return result;
    }

    time_to_full_round = precision_round_down (time_to_full, GPM_UP_TIME_PRECISION);
    time_to_empty_round = precision_round_down (time_to_empty, GPM_UP_TIME_PRECISION);

    if (state == UP_DEVICE_STATE_FULLY_CHARGED) {
        if(percentage >= 100)
            result = tr("fully charged");
        else
            result = tr("charging");
        result = tr("fully charged");

    } else if (state == UP_DEVICE_STATE_DISCHARGING) {

        result = tr("not charging");
    } else if (state == UP_DEVICE_STATE_CHARGING) {
        result = tr("charging");
        
    }  else {
        result = QString("%1(%2%)").arg(kind_desc).arg(percentage);
    }
    return result;
}

/**
 * engine_get_device_summary:
 **/
QString EngineDevice::engine_get_device_summary(DEVICE* dv)
{
    QString kind_desc;
    int time_to_full_round;
    int time_to_empty_round;
    QString time_to_full_str;
    QString time_to_empty_str;
    UpDeviceKind kind;
    UpDeviceState state;
    double percentage;
    bool is_present;
    uint time_to_full;
    uint time_to_empty;

    QString result;

    kind = dv->m_dev.kind;
    is_present = dv->m_dev.IsPresent;
    state = dv->m_dev.State;
    percentage = dv->m_dev.Percentage;

    time_to_empty = dv->m_dev.TimeToEmpty;
    time_to_full = dv->m_dev.TimeToFull;
    if (!is_present)
        return NULL;

    kind_desc = engine_kind_to_localised_text (kind, 1);

    if (state == UP_DEVICE_STATE_FULLY_CHARGED) {

        result = tr("%1% available, charged").arg(percentage);

    } else if (state == UP_DEVICE_STATE_DISCHARGING) {

        result = tr("%1% available, not charging").arg(percentage);

    } else if (state == UP_DEVICE_STATE_CHARGING) {

	time_to_full_round = precision_round_down (time_to_full, GPM_UP_TIME_PRECISION);
	if (time_to_full_round > GPM_UP_TEXT_MIN_TIME)
	{
		time_to_full_str = engine_get_timestring (time_to_full_round);
        	result = tr("%1% available, %2 until charged").arg(percentage).arg(time_to_full_str);
	}
	else
        	result = tr("%1% available, charging").arg(percentage);

    } else if (state == UP_DEVICE_STATE_PENDING_DISCHARGE) {

        /* TRANSLATORS: this is only shown for laptops with multiple batteries */
        result = tr("%1 waiting to discharge (%2%)").arg(kind_desc).arg(percentage);


    } else if (state == UP_DEVICE_STATE_PENDING_CHARGE) {

        /* TRANSLATORS: this is only shown for laptops with multiple batteries */
        result = tr("%1 waiting to charge (%2%)").arg(kind_desc).arg(percentage);

    } else {
        printf ("in an undefined state we are not charging or "
                 "discharging and the batteries are also not charged");
        result = QString("%1 (%2%)").arg(kind_desc).arg(percentage);
    }
    return result;
}


QString EngineDevice::engine_kind_to_string (UpDeviceKind type_enum)
{
    QString ret;
    switch (type_enum) {
    case UP_DEVICE_KIND_LINE_POWER:
        ret = "line-power";
        break;
    case UP_DEVICE_KIND_BATTERY:
        ret = "battery";
        break;
    case UP_DEVICE_KIND_UPS:
        ret = "ups";
        break;
    case UP_DEVICE_KIND_MONITOR:
        ret = "monitor";
        break;
    case UP_DEVICE_KIND_MOUSE:
        ret = "mouse";
        break;
    case UP_DEVICE_KIND_KEYBOARD:
        ret = "keyboard";
        break;
    case UP_DEVICE_KIND_PDA:
        ret = "pda";
        break;
    case UP_DEVICE_KIND_PHONE:
        ret = "phone";
        break;
    case UP_DEVICE_KIND_MEDIA_PLAYER:
        ret = "media-player";
        break;
    case UP_DEVICE_KIND_TABLET:
        ret = "tablet";
        break;
    case UP_DEVICE_KIND_COMPUTER:
        ret = "computer";
        break;
    default:
        ret = "unknown";
    }
    return ret;
}

/**
 * engine_kind_to_localised_text:
 **/
QString EngineDevice::engine_kind_to_localised_text (UpDeviceKind kind, uint number)
{
    Q_UNUSED(number);
    QString text;
    switch (kind) {
    case UP_DEVICE_KIND_LINE_POWER:

        text =  tr("AC adapter");
        break;
    case UP_DEVICE_KIND_BATTERY:
        /* TRANSLATORS: laptop primary battery */
        text =  tr("Laptop battery");
        break;
    case UP_DEVICE_KIND_UPS:
        /* TRANSLATORS: battery-backed AC power source */
        text =  tr("UPS");
        break;
    case UP_DEVICE_KIND_MONITOR:
        /* TRANSLATORS: a monitor is a device to measure voltage and current */
        text =  tr("Monitor");
        break;
    case UP_DEVICE_KIND_MOUSE:
        /* TRANSLATORS: wireless mice with internal batteries */
        text =  tr("Mouse");
        break;
    case UP_DEVICE_KIND_KEYBOARD:
        /* TRANSLATORS: wireless keyboard with internal battery */
        text =  tr("Keyboard");
        break;
    case UP_DEVICE_KIND_PDA:
        /* TRANSLATORS: portable device */
        text =  tr("PDA");
        break;
    case UP_DEVICE_KIND_PHONE:
        /* TRANSLATORS: cell phone (mobile...) */
        text =  tr("Cell phone");
        break;
    case UP_DEVICE_KIND_MEDIA_PLAYER:
        /* TRANSLATORS: media player, mp3 etc */
        text =  tr("Media player");
        break;
    case UP_DEVICE_KIND_TABLET:
        /* TRANSLATORS: tablet device */
        text =  tr("Tablet");
        break;
    case UP_DEVICE_KIND_COMPUTER:
        /* TRANSLATORS: tablet device */
        text =  tr("Computer");
        break;
    default:
        printf ("enum unrecognised: %i", kind);
        text = tr ("unrecognised");
    }
    return text;
}


QString EngineDevice::engine_get_device_icon_index (qreal percentage)
{
    if (percentage < 10)
        return "10";
    else if (percentage < 20)
        return "20";
    else if (percentage < 30)
        return "30";
    else if (percentage < 40)
        return "40";
    else if (percentage < 50)
        return "50";
    else if (percentage < 60)
        return "60";
    else if (percentage < 70)
        return "70";
    else if (percentage < 80)
        return "80";
    else if (percentage < 90)
        return "90";
    return "100";
}

/**
 * engine_get_device_icon:
 *
 **/
QString EngineDevice::engine_get_device_icon (DEVICE *device)
{
    QString prefix;
    QString index_str;
    UpDeviceKind kind;
    UpDeviceState state;
    bool is_present;
    QString result;
    qreal percentage;

    kind = device->m_dev.kind;
    state = device->m_dev.State;
    percentage = device->m_dev.Percentage;
    is_present = device->m_dev.IsPresent;
    /* get correct icon prefix */
    prefix = engine_kind_to_string (kind);

    /* get the icon from some simple rules */
    if (kind == UP_DEVICE_KIND_LINE_POWER) {
        result =  ("gpm-ac-adapter");
    } else if (kind == UP_DEVICE_KIND_MONITOR) {
        result =  ("gpm-monitor");
    } else if (kind == UP_DEVICE_KIND_UPS) {
        if (!is_present) {
            /* battery missing */
            result.sprintf ("gpm-%s-missing", prefix.toStdString().c_str());

        } else if (state == UP_DEVICE_STATE_FULLY_CHARGED) {
            result.sprintf ("gpm-%s-100", prefix.toStdString().c_str());

        } else if (state == UP_DEVICE_STATE_CHARGING) {
            index_str = engine_get_device_icon_index (percentage);
            result = QString("gpm-%1-%2-charging").arg(prefix).arg(index_str);

        } else if (state == UP_DEVICE_STATE_DISCHARGING) {
            index_str = engine_get_device_icon_index (percentage);
            result = QString("gpm-%1-%2").arg(prefix).arg(index_str);
        }
    } else if (kind == UP_DEVICE_KIND_BATTERY) {
        if (!is_present) {
            /* battery missing: battery-missing-symbolic*/
            result = QString ("%1-missing-symbolic").arg(prefix);

        } else if (state == UP_DEVICE_STATE_EMPTY) {
            /* battery-empty-symbolic */
            result = QString ("%1-empty-symbolic").arg(prefix);

        } else if (state == UP_DEVICE_STATE_FULLY_CHARGED) {
            /* battery-full-charged-symbolic */
            result = QString ("%1-full-charged-symbolic").arg(prefix);
        } else if (state == UP_DEVICE_STATE_CHARGING) {
            index_str = engine_get_device_icon_index (percentage);
            /* battery-level-percent-charging-symbolic */
            result = QString("%1-level-%2-charging-symbolic").arg(prefix).arg(index_str);

        } else if (state == UP_DEVICE_STATE_DISCHARGING) {
            index_str = engine_get_device_icon_index (percentage);
            result = QString("%1-level-%2-symbolic").arg(prefix).arg(index_str);

        } else if (state == UP_DEVICE_STATE_PENDING_CHARGE) {
            index_str = engine_get_device_icon_index (percentage);
            /* FIXME: do new grey icons */
            result = QString("%1-level-%2-charging-symbolic").arg(prefix).arg(index_str);

        } else if (state == UP_DEVICE_STATE_PENDING_DISCHARGE) {
            index_str = engine_get_device_icon_index (percentage);
            result = QString("%1-level-%2-symbolic").arg(prefix).arg(index_str);

        } else {
            result =  ("battery-missing-symbolic");
        }

    } else if (kind == UP_DEVICE_KIND_MOUSE ||
           kind == UP_DEVICE_KIND_KEYBOARD ||
           kind == UP_DEVICE_KIND_PHONE) {
        if (!is_present) {
            /* battery missing */
            result = QString ("gpm-%1-000").arg(prefix);

        } else if (state == UP_DEVICE_STATE_FULLY_CHARGED) {
            result = QString ("gpm-%1-100").arg(prefix);

        } else if (state == UP_DEVICE_STATE_DISCHARGING) {
            index_str = engine_get_device_icon_index (percentage);
            result = QString("gpm-%1-%2").arg(prefix).arg(index_str);
        } else if (state == UP_DEVICE_STATE_CHARGING) {
            index_str = engine_get_device_icon_index (percentage);
            result = QString("gpm-%1-%2-charging").arg(prefix).arg(index_str);
        }
    }

    /* nothing matched */
    if (result.isNull()) {
        result =  ("dialog-warning");
    }
    return result;
}

QString EngineDevice::engine_get_dev_icon (DEV dev)
{
    QString prefix;
    QString index_str;
    UpDeviceKind kind;
    UpDeviceState state;
    bool is_present;
    QString result;
    qreal percentage;

    kind = dev.kind;
    state = dev.State;
    percentage = dev.Percentage;
    is_present = dev.IsPresent;
    /* get correct icon prefix */
    prefix = engine_kind_to_string (kind);

        if (kind == UP_DEVICE_KIND_BATTERY) {
            if (!is_present) {
                /* battery missing: battery-missing-symbolic*/
                result = QString ("%1-missing-symbolic").arg(prefix);

            } else if (state == UP_DEVICE_STATE_EMPTY) {
                /* battery-empty-symbolic */
                result = QString ("%1-empty-symbolic").arg(prefix);

            } else if (state == UP_DEVICE_STATE_FULLY_CHARGED) {
                /* battery-full-charged-symbolic */
                result = QString ("%1-full-charged-symbolic").arg(prefix);
            } else if (state == UP_DEVICE_STATE_CHARGING) {
                index_str = engine_get_device_icon_index (percentage);
                /* battery-level-percent-charging-symbolic */
                result = QString("%1-level-%2-charging-symbolic").arg(prefix).arg(index_str);

            } else if (state == UP_DEVICE_STATE_DISCHARGING) {
                index_str = engine_get_device_icon_index (percentage);
                result = QString("%1-level-%2-symbolic").arg(prefix).arg(index_str);

            } else if (state == UP_DEVICE_STATE_PENDING_CHARGE) {
                index_str = engine_get_device_icon_index (percentage);
                /* FIXME: do new grey icons */
                result = QString("%1-level-%2-charging-symbolic").arg(prefix).arg(index_str);

            } else if (state == UP_DEVICE_STATE_PENDING_DISCHARGE) {
                index_str = engine_get_device_icon_index (percentage);
                result = QString("%1-level-%2-symbolic").arg(prefix).arg(index_str);

            } else {
                result =  ("battery-missing-symbolic");
            }

        }
    if (result.isNull()) {
        result =  ("dialog-warning");
    }
    return result;
}

QString EngineDevice::engine_get_dev_predict(DEV dev)
{
    QString result;
    QString kind_desc;
    uint time_to_empty_round;
    QString time_to_empty_str;
    UpDeviceKind kind;
    UpDeviceState state;
    double percentage;
    bool is_present;
    uint time_to_full;
    uint time_to_empty;

    kind = dev.kind;
    is_present = dev.IsPresent;
    state = dev.State;
    percentage = dev.Percentage;

    time_to_empty = dev.TimeToEmpty;
    time_to_full = dev.TimeToFull;
    if (!is_present)
        return NULL;

    kind_desc = engine_kind_to_localised_text (kind, 1);

    time_to_empty_round = precision_round_down (time_to_empty, GPM_UP_TIME_PRECISION);

    if (state == UP_DEVICE_STATE_FULLY_CHARGED) {
        result = tr("fully charged");
    } else if (state == UP_DEVICE_STATE_DISCHARGING) {
        if (time_to_empty_round > GPM_UP_TEXT_MIN_TIME) {
            time_to_empty_str = engine_get_timestring (time_to_empty_round);
            result = time_to_empty_str;
        } else {
            result = tr("discharging(%1%)").arg(percentage);
        }
    } else if (state == UP_DEVICE_STATE_CHARGING) {
        result = tr("charging");
    }  else {
        result = QString("%1(%2%)").arg(kind_desc).arg(percentage);
    }
    return result;
}

QString EngineDevice::engine_get_state_text (UpDeviceState state)
{
    QString state_text;
    switch (state) {
    case UP_DEVICE_STATE_CHARGING:
        state_text = tr("charging");
        break;
    case UP_DEVICE_STATE_DISCHARGING:
        state_text = tr("discharging");
        break;
    case UP_DEVICE_STATE_EMPTY:
        state_text = tr("empty");
        break;
    case UP_DEVICE_STATE_FULLY_CHARGED:
        state_text = tr("fully");
        break;
    default:
        state_text = tr("other");
        break;
    }
    return  state_text;
}
