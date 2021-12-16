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


#ifndef SYSDBUSREGISTER_H
#define SYSDBUSREGISTER_H

#include <QObject>
#include <QCoreApplication>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QDBusMessage>
#include <QtDBus/QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>


#include <stdio.h>
class SysdbusRegister : public QObject
{
    Q_OBJECT

    Q_CLASSINFO("D-Bus Interface", "org.ukui.powermanagement.interface")

public:
    explicit SysdbusRegister();

    ~SysdbusRegister();

    /**
     * @brief executeLinuxCmd
     * @return 返回执行命令后返回内容
     * 调用linux终端
     */
    QString executeLinuxCmd(QString strCmd);

private:
    /**
     * @brief controlLogin1Connection
     * 创建与login1的dbus连接
     */
    void controlLogin1Connection(QString );

    QString canControl(const QString );

    /**
     * @brief setSuspendThenHibernate
     * 设置S3转S4时间（2小时）
     */
    void setSuspendThenHibernate();

    //目前用来区分CPU
    /**
     * @brief getCpuInfo
     * 获取CPU信息
     * @return
     */
    int getCpuInfo();

    /**
     * @brief getBacklightFile
     * 查看backlight中文件
     * @return
     */
    QString getBacklightFile(int);

    /**
     * @brief getBacklightFileNum
     * 获取backlight中文件个数
     * @return
     */
    int getBacklightFileNum();

    QProcess mProcess;

    QByteArray mCpuType;

    QSettings *mSusThenHibSet;
    enum policy
    {
        Performance = 0,
        Balance,
        EnergySaving,
    };

    enum cpuModelName
    {
        OTHER = 0,
        Intel,
        ZHAOXIN,
        Phytium,
        Loongson3A5000,
        HUAWEI,
    };

signals:
    Q_SCRIPTABLE void nameChanged(QString);

public slots:

    Q_SCRIPTABLE void ExitService();

    /**
     * @brief RegulateBrightness
     * 调整屏幕亮度
     */
    Q_SCRIPTABLE QString RegulateBrightness(qulonglong);

    /**
     * @brief TurnOffDisplay
     * 关闭显示器
     */
    Q_SCRIPTABLE void TurnOffDisplay();

    /**
     * @brief CpuFreqencyModulation
     * cpu调频策略修改
     */
    Q_SCRIPTABLE void CpuFreqencyModulation(const QString);

    /**
     * @brief GpuFreqencyModulation
     * gpu调频策略修改
     */
    Q_SCRIPTABLE void GpuFreqencyModulation(const int);

    /**
     * @brief LockScreen
     * 锁屏
     */
    Q_SCRIPTABLE void LockScreen();

    /**
     * @brief Hibernate
     * 重写休眠接口，以适应不同项目的休眠需求
     */
    Q_SCRIPTABLE void Hibernate();

    /**
     * @brief Suspend
     * 重写睡眠接口，以适应不同项目的休眠需求
     */
    Q_SCRIPTABLE void Suspend();
    /**
     * @brief PowerOff
     * 关机接口
     */
    Q_SCRIPTABLE void PowerOff();
    /**
     * @brief Reboot
     * 重启接口
     */
    Q_SCRIPTABLE void Reboot();

    /**
     * @brief GetMaxBrightness
     * 获取屏幕亮度的最大值
     */
    Q_SCRIPTABLE qulonglong GetMaxBrightness();

    /**
     * @brief GetMaxBrightness
     * 获取屏幕当前亮度
     */
    Q_SCRIPTABLE int GetBrightness();

    /**
     * @brief SuspendThenHibernate
     * 睡眠转休眠（时间默认设置为2小时）
     */
    Q_SCRIPTABLE void SuspendThenHibernate();

private:
    int modelName;
};

#endif // SYSDBUSREGISTER_H
