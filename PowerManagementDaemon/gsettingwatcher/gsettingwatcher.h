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


#ifndef GSETTINGWATCHER_H
#define GSETTINGWATCHER_H

#include <QFile>
#include "eventwatcher.h"
#include "acwatcher/acwatcher.h"
#include "lowpowerwatcher/lowpowerwatcher.h"
#include "brightnessbutton/buttonwatcher.h"

class GsettingWatcher : public EventWatcher
{
    Q_OBJECT
public:
    GsettingWatcher();
    ~GsettingWatcher();
    /**
     * @brief initGsettingWatcher
     * 初始化GsettingWatcher
     */
    void initGsettingWatcher();

private:
    enum policy
    {
        Performance,
        Balance,
        EnergySaving,
    };
    /*
     * ===============================================
     * 以下为相关成员函数
     * ===============================================
     */

    /**
     * @brief initSettingsConnect
     * 初始化连接
     */
    void initSettingsConnect();

    /**
     * @brief policyAC
     * 电源时使用策略
     */
    void policyAC();

    /**
     * @brief policyBattery
     * 电池时使用策略
     */
    void policyBattery();

    /**
     * @brief getCpuFreqMode
     * 获取CPU支持的调频策略
     */
    void getCpuFreqMode();

    /**
     * @brief setCpuFreq
     * 设置CPU调频
     */
    void setCpuFreq(const int &policy);

    /**
     * @brief setGpuFreq
     * @param policy
     * 设置GPU模式
     */
    void setGpuFreq(const int &policy);

    /**
     * @brief setPolicy
     * @param policyState
     * 根据电池状态设置相应模式
     */
    void setPolicy(const bool &policyState);

    /**
     * @brief setBrightness
     * @param value
     * 电池状态改变时亮度调整
     */
    void setBrightness(const bool &value);

    bool lowBatteryState();

    /*
     * ===============================================
     * 以下为成员变量
     * ===============================================
     */
    /**
     * @brief mPowerPolicyAc
     * AC状态电源策略
     */
    int mPowerPolicyAc;

    /**
     * @brief mPowerPolicyBattery
     * 电池状态电源策略
     */
    int mPowerPolicyBattery;

    /**
     * @brief mOnBatteryAutoSave
     * 低电量时自动开启节能模式
     */
    int mOnBatteryAutoSave;

    /**
     * @brief mLowBatteryAutoSave
     * 低电量时自动开启节能模式
     */
    int mLowBatteryAutoSave;

    /**
     * @brief mLowPrecentState
     * 低电量状态
     */
    bool mLowPrecentState;

    QByteArray buf;

    AcWatcher mAcWatcher;

//    LowPowerWatcher mLowPowerWatcher;

    ButtonWatcher mButtonWatcher;
private slots:
    /**
     * @brief readSettings
     * 读取用户配置信息
     */
    void readSettings();

    /**
     * @brief AcWatcherChanged
     * @param start
     * 供电状态改变
     */
    void AcWatcherChanged(const bool &start);

    /**
     * @brief setLowPowerState
     * @param lowPowerState
     * 更改低电量状态
     */
    void setLowPowerState(bool lowPowerState);

    /**
     * @brief increaseBrightness
     * 增加亮度
     */
    void increaseBrightness();

    /**
     * @brief reduceBrightness
     * 调低亮度
     */
    void reduceBrightness();
};

#endif // GSETTINGWATCHER_H
