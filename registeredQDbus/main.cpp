/*
 * Copyright: 2021, KylinSoft Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU  Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or (at your option)
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

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>

#include <QDebug>
#include <QStandardPaths>
#include "sysdbusregister.h"


int main(int argc, char *argv[]){

    QCoreApplication app(argc, argv);
    app.setOrganizationName("Kylin Team");
    app.setApplicationName("ukui-powermanagement-service");

    QDBusConnection systemBus = QDBusConnection::systemBus();
    if (!systemBus.registerService("org.ukui.powermanagement")){
        qCritical() << "QDbus register service failed reason:" << systemBus.lastError();
        exit(1);
    }

    if (!systemBus.registerObject("/", new SysdbusRegister(), QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)){
        qCritical() << "QDbus register object failed reason:" << systemBus.lastError();
        exit(2);
    }

    return app.exec();
}
