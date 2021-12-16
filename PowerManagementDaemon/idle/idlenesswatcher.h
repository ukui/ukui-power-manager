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


#ifndef IDLENESSWATCHER_H
#define IDLENESSWATCHER_H

#include "eventwatcher.h"

#include <QTimer>

/**
 * @brief The IdlenessWatchcer class
 * 监听空闲的类
 * 空闲后会有三个操作，分别是：
 * 1.降低屏幕亮度
 * 2.关闭显示器
 * 3.定时睡眠/休眠
 */
class IdlenessWatchcer : public EventWatcher
{
    Q_OBJECT
public:
    IdlenessWatchcer();
    ~IdlenessWatchcer();

    /**
     * @brief initIdleWatcher
     * 初始化空闲监控类
     *
     */
    void initIdleWatcher();

private:
    /*
     * ===============================================
     * 以下为初始化相关成员函数
     * ===============================================
     */

    /**
     * @brief initSettingsConnect
     * 初始化GSetting并连接
     */
    void initSettingsConnect();

    /**
     * @brief initTimer
     * 初始化定时器
     */
    void initTimer();

    /**
     * @brief turnOffDisplayTime
     * 关闭显示器时间
     * @return
     */
    int turnOffDisplayTime();

    /**
     * @brief suspendTime
     * 睡眠时间
     * @return
     */
    int suspendTime();
    /*
     * ===============================================
     * 以下为成员变量，包括定时器，亮度值
     * ===============================================
     */

    /**
     * @brief mTurnOffDisplayTimer
     * 关闭显示器的定时器
     */
    QTimer *mTurnOffDisplayTimer;

    /**
     * @brief mReduceBrightnessTimer
     * 降低屏幕亮度的定时器
     */
    QTimer *mReduceBrightnessTimer;

    /**
     * @brief mSuspendTimer
     * 定时休眠/睡眠的定时器
     */
    QTimer *mSuspendTimer;

    /**
     * @brief mTurnOffDisplayTime
     * mTurnOffDisplayTime 毫秒后关闭显示器（电源情况下）
     */
    int mTurnOffDisplayTimeAc;

    /**
     * @brief mTurnOffDisplayTimeBat
     * mTurnOffDisplayTimeBat毫秒后关闭显示器（电池情况下）
     */
    int mTurnOffDisplayTimeBat;

    /**
     * @brief mReduceBrightnessTime
     * mReduceBrightnessTime毫秒后降低屏幕亮度
     */
    int mReduceBrightnessTime;

    /**
     * @brief mSuspendTimeBat
     * mSuspendTimeBat毫秒后睡眠(电池状态下）
     */
    int mSuspendTimeBat;

    /**
     * @brief mSuspendTimeAc
     * mSuspendTimeAc毫秒后睡眠(电源状态下)
     */
    int mSuspendTimeAc;

    /**
     * @brief mSessionTime
     * mSessionTime进入空闲时间
     */
    const int mSessionTime = 60;

private slots:

    /**
     * @brief readSettings
     * 读配置文件
     */
    void readSettings();

    /**
     * @brief timingBegins
     * 开始计时
     */
    void timingBegins(quint32);

    /**
     * @brief timingStop
     * 停止计时
     */
    void timingStop();

    /**
     * @brief reduceBrightnessTiming
     * 降低屏幕亮度计时
     */
    void reduceBrightness();
};

#endif // IDLENESSWATCHER_H
