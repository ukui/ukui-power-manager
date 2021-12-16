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


#ifndef BATTERYINFO_H
#define BATTERYINFO_H

#include <QObject>
#include <QtDBus>
#include <QGSettings>
#include "../common/common.h"
#include "../upowerinfo/upowerinfo.h"
class BatteryInfo : public QObject
{
    Q_OBJECT
public:
    explicit BatteryInfo(QObject *parent = nullptr);

    ~BatteryInfo();

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

    bool IsPresent();

    bool LowBatteryState();

private:
    QDBusInterface *iface;

    QGSettings *mPowerManagementGsettings;

    UPowerInfo *mUpower;

    bool batteryState;
    /**
     * @brief mPercentage
     * 当前电量百分比
     */
    double mPercentage;

    /**
     * @brief mSetPercentageLow
     * 用户设置的低电量百分比
     */
    int mSetPercentageLow;

    /**
     * @brief mPercentageAction
     * 用户设置的极低电量百分比
     */
    int mPercentageAction;

    /**
     * @brief mLowBatteryState
     * 低电量状态
     */
    bool mLowBatteryState;

    bool mVeryLowBatteryState;

    QString mIconMessage;

    void initBatteryInfo();

    void readSettings();

    void getBatteryInfo();

    void dealLowBatteryMessage(const bool &state);

    void dealBatteryIconMessage(const bool &state,const double &precentage);
private slots:
    void dealMessage(void);

signals:
    void LowBatteryChanged(bool);
    void VeryLowBatteryChanged(bool);
    void BatteryIconChanged(QString);
};

#endif // BATTERYINFO_H
