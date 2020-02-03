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

#define PERFORMANCE          0
#define DEFAULT              1
#define SAVEPOWER            2

#define DBUS_SERVICE "org.freedesktop.UPower"
#define DBUS_OBJECT "/org/freedesktop/UPower"
#define DBUS_INTERFACE "org.freedesktop.DBus.Properties"
#define POWER_SETTINGS_SCHEMA "org.ukui.power-manager"
#define POWERPOLICY_MODE_TEXT			"power-policy-mode"



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
        onbattery = res.arguments().takeFirst().toBool();
    }
}

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

void PowerPolicy::onbattery_change_slot(bool flag)
{

    if(flag)
        process(SAVEPOWER);//from ac to battery
    else {
        process(PERFORMANCE);//from battery to ac
    }

}

void PowerPolicy:: set_integer(int argc)
{
    printf("set_integer: %d\n", argc);
}

void PowerPolicy:: set_string(QString argc)
{
    printf("set_string: %s\n", argc.toStdString().data());
}


int PowerPolicy:: return_integer()
{
    int a1, a2;
    a1 = 10;
    a2 = 20;
    return a1 + a2;
}

bool PowerPolicy:: return_bool()
{
    bool result;
    result = true;
    return result;
}

QString PowerPolicy:: return_string()
{
    QString name = "";
    return name;
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

int PowerPolicy:: return_integet_and_set_integer(int argc_1, const QString &argc_2, QString &out1)
{
    int a;
    a = argc_1;
    out1 = argc_2;
    return a;
}

QString PowerPolicy:: return_string_and_set_string(const QString &argc_1)
{
    QString value;
    value = argc_1.toLower();
    return value;
}
int PowerPolicy::power_control(QString power_status, QString power_mode)
{
    if(power_mode == "powersave")
        mode = SAVEPOWER;
    else if(power_mode == ("performance"))
        mode = PERFORMANCE;
    QString cmd;
    cmd = QString("/usr/bin/power_policy.sh") + " " + power_status + " " + power_mode;
    int rv = system(cmd.toStdString().c_str());
    if (WIFEXITED(rv))
    {
         printf("subprocess exited, exit code: %d\n", WEXITSTATUS(rv));
         if (0 == WEXITSTATUS(rv))
         {
              printf("command succeed");
              ret.sprintf("command succeed");
              return 0;
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
}

QString PowerPolicy::control(int opt)
{
    if(opt == SAVEPOWER)
    {
        mode = opt;
        process(mode);

    }
    else if(opt == PERFORMANCE)
    {
        mode = opt;
        process(mode);

    }
    else if(opt == DEFAULT)
    {
        mode = opt;
        process(mode);

    }
    else if(opt == 3)
    {
        mode = opt;
        process(mode);

    }
    else
        ret.sprintf("undefined mode");
    return ret;

}

int PowerPolicy::process(int option)
{
    Q_EMIT ModeChanged(option);
    QString power_status;
    QString power_mode;
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
    QString cmd;
    cmd = QString("/usr/bin/power_policy.sh") + " " + power_status + " " + power_mode;
    int rv = system(cmd.toStdString().c_str());
    if (WIFEXITED(rv))
    {
         printf("subprocess exited, exit code: %d\n", WEXITSTATUS(rv));
         if (0 == WEXITSTATUS(rv))
         {
              printf("command succeed");
              ret.sprintf("command succeed");
              return 0;
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

}

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
