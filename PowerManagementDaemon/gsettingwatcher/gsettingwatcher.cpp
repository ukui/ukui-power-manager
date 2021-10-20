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

#include "gsettingwatcher.h"

GsettingWatcher::GsettingWatcher()
{
    // qDebug()<<"gsetting watcher";
    // initGsettingWatcher();
}

GsettingWatcher::~GsettingWatcher() {}

void GsettingWatcher::initGsettingWatcher()
{
    initSettingsConnect();
    mAcWatcher.initAcWatcher();
    mButtonWatcher.initButtonWatcher();
}

void GsettingWatcher::initSettingsConnect()
{
    readSettings();
    connect(mPowerManagementGsettings, &QGSettings::changed, this, [=](const QString &key) {
        if (key == POWER_POLICY_AC || key == POWER_POLICY_BATTERY) {
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
    // connect(&mLowPowerWatcher, SIGNAL(lowPowerChanged(bool)), this, SLOT(setLowPowerState(bool)));
    connect(&mButtonWatcher, SIGNAL(brightnessDown()), this, SLOT(reduceBrightness()));
    connect(&mButtonWatcher, SIGNAL(brightnessUp()), this, SLOT(increaseBrightness()));
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
        // qDebug()<<"亮度降低";
        reduceBrightness();
    } else {
        // qDebug()<<"亮度增高";
        increaseBrightness();
    }
}

void GsettingWatcher::setLowPowerState(bool lowPowerState)
{
    mLowPrecentState = lowPowerState;
    setPolicy(mAcWatcher.readOnBattery());
    qDebug() << "当前是否为低电量状态：" << mLowPrecentState;
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
        setGpuFreq(EnergySaving);
    } else {
        if (Performance == mPowerPolicyBattery && 0 == mOnBatteryAutoSave) {
            if (0 == mLowBatteryAutoSave) {
                setCpuFreq(
                    mPowerPolicyBattery); //控制面板设置性能模式 使用电池不自动开启平衡模式 低电量不自动开启平衡模式
                setCpuFreq(Performance);
            } else {
                if (!mLowPrecentState) {
                    setCpuFreq(mPowerPolicyBattery); //控制面板设置性能模式 使用电池不自动开启节能模式
                                                     //低电量开启平衡模式 非低电量
                    setGpuFreq(Performance);
                    // qDebug()<<"性能模式";
                } else {
                    setCpuFreq(Balance); //控制面板设置性能模式 低电量自动开启平衡模式 低电量
                    setGpuFreq(Balance);
                    // qDebug()<<"平衡模式";
                }
            }
        } else {
            setCpuFreq(Balance); //控制面板设置性能模式 使用电池自动开启平衡模式 or 控制面板设置平衡模式
            setGpuFreq(Balance);
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
            if (buf.indexOf("schedutil") == -1) {
                if (buf.indexOf("ondemand") == -1) {
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
        // controlPowerManagement(REGULATE_BRIGHTNESS, mBrightness);
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
        // controlPowerManagement(REGULATE_BRIGHTNESS, mBrightness);
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
    buf = file.readAll();
    // qDebug()<<buf;
    file.close();
}
