/*
 * Copyright 2021 KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sysdbusregister.h"

#include <QDebug>
#include <QSharedPointer>
#include <QRegExp>
#include <stdlib.h>
#include <QX11Info>

SysdbusRegister::SysdbusRegister()
{
    if (1 == getBacklightFileNum()) {
        modelName = OTHER;
    } else {
        modelName = getCpuInfo();
    }
    mSusThenHibSet = new QSettings(QString("/etc/systemd/sleep.conf"), QSettings::IniFormat, this);
    mSusThenHibSet->setIniCodec("UTF-8");
}

SysdbusRegister::~SysdbusRegister()
{
    delete  mSusThenHibSet;
}

void SysdbusRegister::ExitService()
{
    qApp->exit(0);
}

QString SysdbusRegister::executeLinuxCmd(QString strCmd)
{
    //调用linux终端命令
    mProcess.start("bash", QStringList() << "-c" << strCmd);
    mProcess.waitForFinished();
    QString strResult = mProcess.readAllStandardOutput() + mProcess.readAllStandardError();
    return strResult;
}

QString SysdbusRegister::canControl(const QString control)
{
    QDBusInterface iface(
        "org.freedesktop.login1",
        "/org/freedesktop/login1",
        "org.freedesktop.login1.Manager",
        QDBusConnection::systemBus());
    QDBusReply<QString> reply = iface.call(control);
    if (reply.isValid()) {
        if ("yes" == reply.value()) {
            return reply;
        } else {
            QDBusMessage message = QDBusMessage::createSignal("/", "org.ukui.powermanagement.interface", "CanControl");
            message << reply.value();
            QDBusConnection::systemBus().send(message);
            return reply;
        }
    } else {
        return "error";
    }
}

void SysdbusRegister::controlLogin1Connection(QString type)
{
    bool ctrl = 1;
    QDBusInterface iface(
        "org.freedesktop.login1",
        "/org/freedesktop/login1",
        "org.freedesktop.login1.Manager",
        QDBusConnection::systemBus());
    iface.call(type, ctrl);
}

qulonglong SysdbusRegister::GetMaxBrightness()
{
    qulonglong maxValue;
    switch (modelName) {
        case ZHAOXIN:
            maxValue = executeLinuxCmd("cat /sys/class/backlight/acpi_video1/max_brightness").toInt();
            break;
        case Phytium:
            maxValue = executeLinuxCmd("cat /sys/class/backlight/ec_bl/max_brightness").toInt();
            break;
        case Loongson3A5000:
            maxValue = executeLinuxCmd(
                           QString("cat /sys/class/backlight/%1/max_brightness").arg(getBacklightFile(Loongson3A5000)))
                           .toInt();
            break;
        case OTHER:
            maxValue = executeLinuxCmd("cat /sys/class/backlight/*/max_brightness").toInt();
            break;
        default:
            break;
    }
    return maxValue;
}

int SysdbusRegister::GetBrightness()
{
    int value;
    switch (modelName) {
        case ZHAOXIN:
            value = executeLinuxCmd("cat /sys/class/backlight/acpi_video1/brightness").toInt();
            break;
        case Phytium:
            value = executeLinuxCmd("cat /sys/class/backlight/ec_bl/brightness").toInt();
            break;
        case Loongson3A5000:
            value = executeLinuxCmd(
                        QString("cat /sys/class/backlight/%1/brightness").arg(getBacklightFile(Loongson3A5000)))
                        .toInt();
            break;
        case OTHER:
            value = executeLinuxCmd("cat /sys/class/backlight/*/brightness").toInt();
            break;
        default:
            break;
    }
    return value;
}

QString SysdbusRegister::RegulateBrightness(qulonglong brightness)
{
    QString msg;
    switch (modelName) {
        case ZHAOXIN:
            break;
        case Phytium:
            msg = executeLinuxCmd(QString("echo %1 | tee /sys/class/backlight/ec_bl/brightness").arg(brightness));
            break;
        case Loongson3A5000:
            msg = executeLinuxCmd(QString("echo %1 | tee /sys/class/backlight/%2/brightness")
                                      .arg(brightness)
                                      .arg(getBacklightFile(Loongson3A5000)));
            break;
        case OTHER:
            msg = executeLinuxCmd(QString("echo %1 | tee /sys/class/backlight/*/brightness").arg(brightness));
            break;
        default:
            break;
    }
    return msg;
}

QString SysdbusRegister::getBacklightFile(int type)
{
    if (Loongson3A5000 == type) {
        QFile loongsonGpuFile("/sys/class/backlight/loongson-gpu");
        if (loongsonGpuFile.exists()) {
            return QString("loongson-gpu");
        } else {
            return QString("loongson_laptop");
        }
    } else {
        return QString("*");
    }
}

int SysdbusRegister::getBacklightFileNum()
{
    QDir dir("/sys/class/backlight");
    QStringList filter;
    QStringList filterAll;
    filter << ".*";
    dir.setNameFilters(filter);
    QList<QFileInfo> fileInfo = dir.entryInfoList(filter);
    int num = fileInfo.count();
    fileInfo = dir.entryInfoList(filterAll);
    int numAll = fileInfo.count();
    return numAll - num;
}

void SysdbusRegister::TurnOffDisplay()
{
    //关闭显示器由后台处理
}

void SysdbusRegister::CpuFreqencyModulation(QString strategy)
{
    int cpuQuantity = executeLinuxCmd("grep -c 'processor' /proc/cpuinfo").toInt();
    for (int var = 0; var < cpuQuantity; ++var) {
        executeLinuxCmd(
            QString("echo %1 | tee /sys/devices/system/cpu/cpu%2/cpufreq/scaling_governor").arg(strategy).arg(var));
    }
}

void SysdbusRegister::GpuFreqencyModulation(int strategy)
{
    QFile radeonFile("/sys/class/drm/card0/device/power_dpm_state");
    QFile amdgpuFile("/sys/class/drm/card0/device/power_dpm_force_performance_level");
    if (radeonFile.exists()) {
        switch (strategy) {
            case Performance:
                executeLinuxCmd(QString("echo performance | tee /sys/class/drm/card0/device/power_dpm_state"));
                break;
            case Balance:
                executeLinuxCmd(QString("echo balanced | tee /sys/class/drm/card0/device/power_dpm_state"));
                break;
            case EnergySaving:
                executeLinuxCmd(QString("echo battery | tee /sys/class/drm/card0/device/power_dpm_state"));
                break;
            default:
                break;
        }
    }
    if (amdgpuFile.exists()) {
        switch (strategy) {
            case Performance:
                executeLinuxCmd(
                    QString("echo high | tee /sys/class/drm/card0/device/power_dpm_force_performance_level"));
                break;
            case Balance:
                executeLinuxCmd(
                    QString("echo auto | tee /sys/class/drm/card0/device/power_dpm_force_performance_level"));
                break;
            case EnergySaving:
                executeLinuxCmd(
                    QString("echo low | tee /sys/class/drm/card0/device/power_dpm_force_performance_level"));
                break;
            default:
                break;
        }
    }
}

void SysdbusRegister::LockScreen()
{
    //锁屏目前由后台处理
}

void SysdbusRegister::Hibernate()
{
    if ("yes" == canControl("CanHibernate")) {
        controlLogin1Connection("Hibernate");
    }
}

void SysdbusRegister::Suspend()
{
    if ("yes" == canControl("CanSuspend")) {
        controlLogin1Connection("Suspend");
    }
}

void SysdbusRegister::PowerOff()
{
    if ("yes" == canControl("CanPowerOff")) {
        controlLogin1Connection("PowerOff");
    }
}

void SysdbusRegister::Reboot()
{
    if ("yes" == canControl("CanReboot")) {
        controlLogin1Connection("Reboot");
    }
}

void SysdbusRegister::SuspendThenHibernate()
{
    if ("yes" == canControl("CanSuspendThenHibernate")) {
        mSusThenHibSet->beginGroup("Sleep");
        mSusThenHibSet->setValue("HibernateDelaySec", 7200);
        mSusThenHibSet->endGroup();
        mSusThenHibSet->sync();
        controlLogin1Connection("SuspendThenHibernate");
    }
}

int SysdbusRegister::getCpuInfo()
{
    QFile file("/proc/cpuinfo");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
    }
    mCpuType = file.readAll();
    file.close();
    if (-1 != mCpuType.indexOf("ZHAOXIN")) {
        return ZHAOXIN;
    } else if (-1 != mCpuType.indexOf("D2000")) {
        return Phytium;
    } else if (-1 != mCpuType.indexOf("3A5000")) {
        return Loongson3A5000;
    } else if (-1 != mCpuType.indexOf("FT-2000")) {
        return Phytium;
    } else {
        return OTHER;
    }
}
