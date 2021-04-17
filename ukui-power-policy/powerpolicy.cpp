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
#include "powerpolicy.h"
#include <stdio.h>
#include <stdlib.h>
#include "dbus_struct.h"
#include <QDBusConnection>
#include <QDBusArgument>
#include <QDebug>
#include "policy_config.h"

#define PERFORMANCE          0
#define DEFAULT              1
#define POWERSAVE            2

#define DBUS_SERVICE "org.freedesktop.UPower"
#define DBUS_OBJECT "/org/freedesktop/UPower"
#define DBUS_INTERFACE "org.freedesktop.DBus.Properties"

/**
 * PowerPolicy:
 * main class construction. connecting to service of org.freedesktop.UPower. getting the status of onbattery.
 **/
PowerPolicy::PowerPolicy(QObject *parent) : QObject(parent)
{

    connect(this,SIGNAL(onbattery_change(bool)),this,SLOT(onbattery_change_slot(bool)));
    QDBusConnection::systemBus().connect(DBUS_SERVICE,DBUS_OBJECT,DBUS_INTERFACE,
                                         QString("PropertiesChanged"),this,SLOT(onPropertiesSlot(QDBusMessage)));
    QDBusMessage msg = QDBusMessage::createMethodCall(DBUS_SERVICE,DBUS_OBJECT,DBUS_INTERFACE,"Get");
    msg<<DBUS_SERVICE<<"OnBattery";
    QDBusMessage res = QDBusConnection::systemBus().call(msg);
    if(res.type()==QDBusMessage::ReplyMessage)
    {
        QVariant v = res.arguments().at(0);
        QDBusVariant dv = qvariant_cast<QDBusVariant>(v);
        QVariant result = dv.variant();
        onbattery = result.toBool();
    }
}

/**
 * onPropertiesSlot:
 * @msg: dbus message from connection.
 *
 * when changes ,getting the status of onbattery
 **/
void PowerPolicy::onPropertiesSlot(QDBusMessage msg)
{
    const QDBusArgument &arg = msg.arguments().at(1).value<QDBusArgument>();
    QMap<QString,QVariant> map;
    arg >> map;
    if(map.contains("OnBattery"))
    {
        onbattery=map.value(QString("OnBattery")).toBool();
        Q_EMIT onbattery_change(onbattery);
    }

}

/**
 * onbattery_change_slot:
 * @flag: the battery power suply flag. true for power on battery.
 *
 * this function is called on receiving signal.
 **/
void PowerPolicy::onbattery_change_slot(bool flag)
{
    int option = 0;
    if (!enable_auto)
    {
        return;
    }

    /**change mode**/
    if(flag)
    {
        process(POWERSAVE);//from ac to battery
        option = POWERSAVE;
    }
    else
    {
        process(PERFORMANCE);//from battery to ac
        option = PERFORMANCE;
    }
    Q_EMIT ModeChanged(option);

}

/**
 * change_auto_switch:
 * @flag: the enable auto switch flag.
 *
 * this function is called by dbus function.
 **/
bool PowerPolicy::change_auto_switch(bool flag)
{
    if (enable_auto != flag)
    {

        enable_auto = flag;
        return true;
    }
    return false;
}

/**
 * control:
 * @opt: the chosen mode by user.
 *
 * this function is called when user chooses an power selection.
 **/
QString PowerPolicy::control(int opt)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(DBUS_SERVICE,DBUS_OBJECT,DBUS_INTERFACE,"Get");
    msg<<DBUS_SERVICE<<"OnBattery";
    QDBusMessage res = QDBusConnection::systemBus().call(msg);
    if(res.type()==QDBusMessage::ReplyMessage)
    {
        QVariant v = res.arguments().at(0);
        QDBusVariant dv = qvariant_cast<QDBusVariant>(v);
        QVariant result = dv.variant();
        onbattery = result.toBool();
    }

    process(opt);
    return ret;

}

/**
 * control:
 * @option: the chosen mode by user.
 *
 * this function is to call those exact device save function.
 **/
int PowerPolicy::process(int option)
{
    if(onbattery)
    {
        desire_status = 2;
    }
    else
    {
        desire_status = 1;
    }
    if(option==0)//外部０是性能，内部１是性能。外部2表示内部省电２。
    {
        desire_mode = 1;
    }
    else if (option==2) {
        desire_mode = 2;
    }
    else {
	return -1;
    }

    int status = desire_status;
    int mode = desire_mode;
    set_cpu_scaling_gover_speed_mode(status,mode);
    //set_drm_mode(status,mode);
    set_pcie_aspm_mode(status,mode);
    set_sata_alsm_mode(status,mode);
    set_audio_mode(status,mode);

    return 0;
}

/**取消了执行前对使用电池的检测，允许ac选择两种模式**/
//int PowerPolicy::process(int option)
//{
//    if(onbattery)
//    {
//        desire_status = 2;
//        if(option==0)
//        {
//            desire_mode = 1;
//        }
//        else {
//            desire_mode = 2;
//        }
//    }
//    else {
//        desire_status = 1;
//        desire_mode = 1;
//    }

//    int status = desire_status;
//    int mode = desire_mode;
//    set_cpu_scaling_gover_speed_mode(status,mode);
//    set_drm_mode(status,mode);
//    set_pcie_aspm_mode(status,mode);
//    set_sata_alsm_mode(status,mode);
//    set_audio_mode(status,mode);

//    return 0;
//}

/**
 * process_shell:
 * @option: the chosen mode by user.
 *
 * this function is to call a shell script.
 **/
int PowerPolicy::process_shell(int option)
{
    Q_EMIT ModeChanged(option);
    QString power_status;
    QString power_mode;
    QString cmd;
    if(onbattery)
        power_status = "BAT";
    else {
        power_status = "AC";
    }

    if(option==0)
    {
        power_mode = "PERFORMANCE";
    }
    else {
        power_mode = "POWERSAVE";
    }
    cmd = QString("/usr/bin/power_policy.sh") + " " + power_status + " " + power_mode;
    int rv = system(cmd.toStdString().c_str());
    if (WIFEXITED(rv))
    {
         if (0 == WEXITSTATUS(rv))
         {
              ret.sprintf("command succeed");
              return 0;
         }
         else
         {
              if(127 == WEXITSTATUS(rv))
              {
                   ret.sprintf("command not found");
                   return WEXITSTATUS(rv);
              }
              else
              {
                   ret.sprintf("command failed");
                   return WEXITSTATUS(rv);
              }
         }
     }
    else
    {
         ret.sprintf("subprocess exit failed");
         return -1;
    }

}

/**
 * script_process:
 * @cmd: command to call.
 *
 * this function is to call a system function.
 **/
int PowerPolicy::script_process(QString cmd)
{
    int rv = system(cmd.toStdString().c_str());
    if (WIFEXITED(rv))
    {
         printf("subprocess exited, exit code: %d\n", WEXITSTATUS(rv));
         if (0 == WEXITSTATUS(rv))
         {
              // if command returning 0 means succeed
              printf("command succeed");
              ret.sprintf("command succeed");
         }
         else
         {
              if(127 == WEXITSTATUS(rv))
              {
                   printf("command not found\n");
                   ret.sprintf("command not found");
                   return WEXITSTATUS(rv);
              }
              else
              {
                   printf("command failed: %s\n", strerror(WEXITSTATUS(rv)));
                   ret.sprintf("command failed");
                   return WEXITSTATUS(rv);
              }
         }
     }
    else
    {
         printf("subprocess exit failed");
         ret.sprintf("subprocess exit failed");
         return -1;
    }
    return -1;

}

QVariantList PowerPolicy::return_variantlist()
{
    QVariantList value;
    QVariant cnt;
    struct dbus_demo_example_struct demo[3];
    for(int i = 0; i < 3; i++)
    {
        demo[i].drv_ID = 1;
        demo[i].name   = "";
        demo[i].full_name = "";
        demo[i].notify_mid = 2;
        cnt = QVariant::fromValue(demo[i]);
        value << cnt;
    }
    return value;
}

