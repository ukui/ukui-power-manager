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

#ifndef DBUS_H
#define DBUS_H
#include "upowerinfo/upowerinfo.h"
#include "batteryinfo/batteryinfo.h"

#include <QObject>
#include <QtDBus>

class DBus : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface","org.ukui.upower")
public:
    explicit DBus(QObject *parent = 0);

    ~DBus();

public slots:
    /**
     * @brief UPowerVersion
     * ukui-upower版本
     * @return
     */
    QString UPowerVersion(void);

    /**
     * @brief CanHibernate
     * @return
     * 是否可以休眠的接口
     * 或可直接调用login1 的
     * 重新封装的意义在于可以针对部分机型做特殊处理
     * 并且可以
     */
    QString CanHibernate(void);

    /**
     * @brief MachineType
     * @return
     * 机器类型判断
     */
    QString MachineType();

    /**
     * @brief OnBattery
     * AC状态还是DC状态
     * @return
     */
    bool OnBattery();

    /**
     * @brief DaemonVersion
     * 判断upower版本
     * @return
     */
    QString DaemonVersion();

    /**
     * @brief IconName
     * @return
     * 图标名称
     */
    QString IconName();

    /**
     * @brief Percentage
     * @return
     * 电量百分比
     */
    double Percentage();

    /**
     * @brief TimeToFull
     * @return
     * 充电过程中，还有多久充满电
     */
    int TimeToFull();

    /**
     * @brief TimeToEmpty
     * @return
     * 放电过程 还有多久耗尽
     */
    int TimeToEmpty();

    /**
     * @brief IsPresent
     * 电池是否存在
     * @return
     */
    bool IsPresent();

    /**
     * @brief State
     * @return
     */
    int State();

    /**
     * @brief LowBattery
     * 低电量
     * @return
     */
    bool LowBattery();

signals:
    /**
     * @brief lowBatteryState
     * @return
     * 低电量状态信号
     */
    void LowBatteryState(bool);

    /**
     * @brief VeryLowBatteryState
     */
    void VeryLowBatteryState(bool);

    /**
     * @brief PowerState
     */
    void PowerState(bool);

    /**
     * @brief LidState
     */
    void LidState(bool);

    void BatteryIcon(QString);

private:
    UPowerInfo *upowerinfo;
    BatteryInfo *batteryinfo;

    void dealPowerState(bool);

    void dealLidState(bool);

    void dealLowBattery(bool);

    void dealVeryLowBattery(bool);

    void dealBatteryIcon(QString);
};

#endif // DBUS_H
