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

#include "idlenesswatcher.h"

IdlenessWatchcer::IdlenessWatchcer()
{

}

IdlenessWatchcer::~IdlenessWatchcer()
{
    delete mTurnOffDisplayTimer;
    delete mReduceBrightnessTimer;
    delete mSuspendTimer;
}

void IdlenessWatchcer::initIdleWatcher()
{
    //初始化gsettings,同时读配置文件
    initSettingsConnect();

    initTimer();

    //监听到空闲信号，开始计时，启动timingBegins函数
    QDBusConnection::sessionBus().connect(
        QString(),
        QString(SESSION_MANAGER_PATH),
        GNOME_SESSION_MANAGER,
        "StatusChanged",
        this,
        SLOT(timingBegins(quint32)));

    QDBusConnection::systemBus().connect("org.freedesktop.login1",
                                         "/org/freedesktop/login1",
                                         "org.freedesktop.login1.Manager",
                                         "PrepareForSleep",
                                         this,
                                         SLOT(timingStop()));
    connect(mTurnOffDisplayTimer, &QTimer::timeout, [this] {
        turnOffDisplay();
        mTurnOffDisplayTimer->stop();
    });
    connect(mReduceBrightnessTimer, &QTimer::timeout, [this] {
        reduceBrightness();
        mReduceBrightnessTimer->stop();
    });
    connect(mSuspendTimer, &QTimer::timeout, [this] {
        suspend();
        mSuspendTimer->stop();
    });
    //监听到非空闲信号，打断计时
}

void IdlenessWatchcer::initSettingsConnect()
{
    readSettings();
    connect(mPowerManagementGsettings, &QGSettings::changed, this, [=] { readSettings(); });
}

void IdlenessWatchcer::initTimer()
{
    mTurnOffDisplayTimer = new QTimer;
    mReduceBrightnessTimer = new QTimer;
    mSuspendTimer = new QTimer;
}

void IdlenessWatchcer::readSettings()
{
    //读配置文件
    mTurnOffDisplayTimeBat = mPowerManagementGsettings->get(SLEEP_DISPLAY_BAT_KEY).toInt();
    qDebug() << "mTurnOffDisplayTimeBat" << mTurnOffDisplayTimeBat;
    mTurnOffDisplayTimeAc = mPowerManagementGsettings->get(SLEEP_DISPLAY_AC_KEY).toInt();
    qDebug() << "mTurnOffDisplayTimeAc" << mTurnOffDisplayTimeAc;
    mReduceBrightnessTime = mPowerManagementGsettings->get(IDLE_DIM_TIME).toInt();
    qDebug() << "mReduceBrightnessTime" << mReduceBrightnessTime;
    mSuspendTimeAc = mPowerManagementGsettings->get(SLEEP_COMPUTER_AC).toInt();
    qDebug() << "mSuspendTime" << mSuspendTimeAc;
    mSuspendTimeBat = mPowerManagementGsettings->get(SLEEP_COMPUTER_BATTERY).toInt();
}

void IdlenessWatchcer::timingBegins(quint32 time)
{
    int mTurnOffDisplayTime = turnOffDisplayTime() - mSessionTime;
    int mSuspendTime = suspendTime() - mSessionTime;
    int mBrightnessTime = -1;
    qDebug() << "Idle signal received from SessionManager" << time;
    if (0 != time) {
        if (mReduceBrightnessTime >= mSessionTime) {
           mBrightnessTime = mReduceBrightnessTime - mSessionTime;
        }
        if (0 <= mTurnOffDisplayTime) {
            mTurnOffDisplayTimer->start(mTurnOffDisplayTime * 1000);
            if (mBrightnessTime < mTurnOffDisplayTime && 0 <= mBrightnessTime) {
                mReduceBrightnessTimer->start(mBrightnessTime * 1000);
            }
        } else {
            qDebug() << "TurnOffDisplayTimer not started!";
            if (0 <= mBrightnessTime) {
                mReduceBrightnessTimer->start(mBrightnessTime * 1000);
            } else {
                qDebug() << "ReduceBrightnessTimer not started!";
            }
        }
        if (0 < mSuspendTime) {
            mSuspendTimer->start(mSuspendTime * 1000);
        } else {
            qDebug() << "SuspendTimer not started!";
        }
    } else {
        timingStop();
        getUserBrightness();
        controlPowerManagement(REGULATE_BRIGHTNESS, mUserBrightness);
    }
}

int IdlenessWatchcer::turnOffDisplayTime()
{
    QDBusInterface iface(UKUI_UPOWER_SERVICE, UKUI_UPOWER_PATH, UKUI_UPOWER_INTERFACE, QDBusConnection::sessionBus());
    QDBusReply<bool> reply = iface.call("OnBattery");
    if (reply.isValid()) {
        if (reply.value()) {
            qDebug() << "Time to turn off the display in battery state" << mTurnOffDisplayTimeBat;
            return mTurnOffDisplayTimeBat;
        } else {
            qDebug() << "Time to turn off the display in power state" << mTurnOffDisplayTimeAc;
            return mTurnOffDisplayTimeAc;
        }
    } else {
        qDebug() << "turn off display time get OnBattery error!";
        return -1;
    }
}

int IdlenessWatchcer::suspendTime()
{
    QDBusInterface iface(UKUI_UPOWER_SERVICE, UKUI_UPOWER_PATH, UKUI_UPOWER_INTERFACE, QDBusConnection::sessionBus());
    QDBusReply<bool> reply = iface.call("OnBattery");
    if (reply.isValid()) {
        if (reply.value()) {
            qDebug() << "Time to suspend in battery state" << mSuspendTimeBat;
            return mSuspendTimeBat;
        } else {
            qDebug() << "Time to suspend in power state" << mSuspendTimeAc;
            return mSuspendTimeAc;
        }
    } else {
        qDebug() << "suspend time get OnBattery error!";
        return -1;
    }
}

void IdlenessWatchcer::timingStop()
{
    qDebug() << "timing stop";
    if (mTurnOffDisplayTimer->isActive()) {
        mTurnOffDisplayTimer->stop();
    } if (mSuspendTimer->isActive()) {
        mSuspendTimer->stop();
    } if (mReduceBrightnessTimer->isActive()) {
        mReduceBrightnessTimer->stop();
    }
}

void IdlenessWatchcer::reduceBrightness()
{
    if (0.1 < mPowerManagementGsettings->get(BRIGHTNESS_AC).toDouble()) {
        getUserBrightness();
        mBrightness = mUserBrightness * 0.5;
        controlPowerManagement(REGULATE_BRIGHTNESS, mBrightness);
        qDebug() << "After idle, the brightness value is:" << mBrightness;
    } else {
        qDebug() << "Brightness is the lowest value";
    }
}
