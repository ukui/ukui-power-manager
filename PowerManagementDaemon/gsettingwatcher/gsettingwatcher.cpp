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

#include "gsettingwatcher.h"

GsettingWatcher::GsettingWatcher() {}

GsettingWatcher::~GsettingWatcher() {}

void GsettingWatcher::initGsettingWatcher()
{
    initSettingsConnect();
    mAcWatcher.initAcWatcher();
}

void GsettingWatcher::initSettingsConnect()
{
    readSettings();
    connect(mPowerManagementGsettings, &QGSettings::changed, this, [=](const QString &key) {
        if (key == POWER_POLICY_AC || key == POWER_POLICY_BATTERY || key == LOW_BATTERY_ATUO_SAVE) {
            readSettings();
            setPolicy(mAcWatcher.mPowerState);
        }
    });
    setPolicy(mAcWatcher.mPowerState);
    connect(&mAcWatcher, SIGNAL(acChanged(bool)), this, SLOT(AcWatcherChanged(bool)));
    QDBusConnection::sessionBus().connect(
        UKUI_UPOWER_SERVICE,
        UKUI_UPOWER_PATH,
        UKUI_UPOWER_INTERFACE,
        "LowBatteryState",
        this,
        SLOT(setLowPowerState(bool)));
    setLowPowerState(lowBatteryState());
}

void GsettingWatcher::readSettings()
{
    mPowerPolicyAc = mPowerManagementGsettings->get(POWER_POLICY_AC).toInt();
    mPowerPolicyBattery = mPowerManagementGsettings->get(POWER_POLICY_BATTERY).toInt();
    qDebug() << "policy ac :" << mPowerPolicyAc << "policy battery:" << mPowerPolicyBattery;
    mOnBatteryAutoSave = mPowerManagementGsettings->get(ON_BATTERY_AUTO_SAVE).toInt();
    mLowBatteryAutoSave = mPowerManagementGsettings->get(LOW_BATTERY_ATUO_SAVE).toInt();
    qDebug() << "On Battery Auto Save:" << mOnBatteryAutoSave << "Low Battery Auto Save:" << mLowBatteryAutoSave;
}

void GsettingWatcher::AcWatcherChanged(const bool &start)
{
    setPolicy(start);
    setBrightness(start);
}

void GsettingWatcher::setPolicy(const bool &policyState)
{
    if (true == policyState) {
        qDebug() << "power state is battery";
        policyBattery();
    } else {
        qDebug() << "power state is ac";
        policyAC();
    }
}

void GsettingWatcher::setBrightness(const bool &value)
{
    if (true == value) {
        // 亮度降低
        reduceBrightness();
    } else {
        // 亮度增高
        increaseBrightness();
    }
}

void GsettingWatcher::setLowPowerState(bool lowPowerState)
{
    mLowPrecentState = lowPowerState;
    setPolicy(mAcWatcher.readOnBattery());
    qDebug() << "Is it currently in a low battery state：" << mLowPrecentState;
}

void GsettingWatcher::policyAC()
{
    setCpuFreq(mPowerPolicyAc);
    setGpuFreq(mPowerPolicyAc);
}

void GsettingWatcher::policyBattery()
{
    if (EnergySaving == mPowerPolicyBattery) {
        setCpuFreq(mPowerPolicyBattery); //控制面板设置节能模式
        setGpuFreq(mPowerPolicyBattery);
    } else {
        if (0 == mLowBatteryAutoSave || !mLowPrecentState) {
            setCpuFreq(mPowerPolicyBattery); //低电量不自动开启平衡模式 或 非低电量
            setCpuFreq(mPowerPolicyBattery);
        } else {
            setCpuFreq(EnergySaving); //低电量自动开启省电模式 低电量
            setGpuFreq(EnergySaving);
        }
    }
}

void GsettingWatcher::setCpuFreq(const int &policy)
{
    switch (policy) {
        case Performance:
            controlPowerManagement(CPU_FREQENCY_MODULATION, "performance");
            break;
        case Balance:
            getCpuFreqMode();
            if (-1 == mCpuMode.indexOf("schedutil")) {
                if (-1 == mCpuMode.indexOf("ondemand")) {
                    controlPowerManagement(CPU_FREQENCY_MODULATION, "powersave");
                } else {
                    controlPowerManagement(CPU_FREQENCY_MODULATION, "ondemand");
                }
            } else {
                controlPowerManagement(CPU_FREQENCY_MODULATION, "schedutil");
            }
            break;
        case EnergySaving:
            controlPowerManagement(CPU_FREQENCY_MODULATION, "powersave");
            break;
        default:
            break;
    }
}

void GsettingWatcher::setGpuFreq(const int &policy)
{
    // qDebug()<<"设置GPU频率";
    controlPowerManagement(GPU_FREQENCY_MODULATION, policy);
}

bool GsettingWatcher::lowBatteryState()
{
    bool value;
    QDBusInterface iface(UKUI_UPOWER_SERVICE, UKUI_UPOWER_PATH, UKUI_UPOWER_INTERFACE, QDBusConnection::sessionBus());
    QDBusReply<bool> reply = iface.call("LowBattery");
    if (reply.isValid()) {
        value = reply.value();
    } else {
        value = false;
        qDebug() << "low battery state error!";
    }
    return value;
}

void GsettingWatcher::reduceBrightness()
{
    if (10.0 < mPowerManagementGsettings->get(BRIGHTNESS_AC).toDouble()) {
        mMaxBrightness = controlPowerManagement(GET_MAX_BRIGHTNESS);
        mUserBrightness = mPowerManagementGsettings->get(BRIGHTNESS_AC).toDouble() - 10;
        if (10 > mUserBrightness) {
            mUserBrightness = 10;
        }
        mBrightness = mUserBrightness * 0.01 * mMaxBrightness;
        mPowerManagementGsettings->set(BRIGHTNESS_AC, mUserBrightness);
        qDebug() << "the brightness value is:" << mBrightness;
    } else {
        qDebug() << "Brightness is the lowest value";
    }
}

void GsettingWatcher::increaseBrightness()
{
    if (100.0 > mPowerManagementGsettings->get(BRIGHTNESS_AC).toDouble()) {
        mMaxBrightness = controlPowerManagement(GET_MAX_BRIGHTNESS);
        mUserBrightness = mPowerManagementGsettings->get(BRIGHTNESS_AC).toDouble() + 10;
        if (100 < mUserBrightness) {
            mUserBrightness = 100;
        }
        mBrightness = mUserBrightness * 0.01 * mMaxBrightness;
        mPowerManagementGsettings->set(BRIGHTNESS_AC, mUserBrightness);
        qDebug() << "the brightness value is:" << mBrightness;
    } else {
        qDebug() << "Brightness is the highest value";
    }
}

void GsettingWatcher::getCpuFreqMode()
{
    QFile file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
    }
    mCpuMode = file.readAll();
    file.close();
}
