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


#ifndef EVENTWATCHER_H
#define EVENTWATCHER_H

#include <QObject>
#include <QProcess>
#include <QGSettings>
#include "common.h"
class EventWatcher : public QObject
{
    Q_OBJECT
public:
    EventWatcher();
    ~EventWatcher();
    /**
     * @brief initEventWatcher
     * 初始化
     */
    void initEventWatcher();

    /**
     * @brief dealBrightnessMsg
     * 处理亮度信息
     */
    void dealBrightnessMsg();

    /**
     * @brief initSettings
     * 获取用户设置亮度值
     */
    void getUserBrightness();

    /**
     * @brief initConfig
     * 设置亮度值
     */
    void setBrightness();

    /**
     * @brief controlPowerManagement
     * 控制电源管理dbus建立
     */
    int controlPowerManagement(const QString &type);
    void controlPowerManagement(const QString &type, const int &value);
    void controlPowerManagement(const QString &type, const QString &contrl);
    void controlPowerManagement(const QString &type, const qulonglong &brightness);

    /**
     * @brief turnOffDisplay
     * 关闭显示器
     */
    void turnOffDisplay();

    /**
     * @brief turnOnDisplay
     * 打开显示器
     */
    void turnOnDisplay();

    /**
     * @brief lock
     * 锁屏
     */
    void lock();

    /**
     * @brief suspend
     * 睡眠
     */
    void suspend();

    /**
     * @brief hibernate
     * 休眠
     */
    void hibernate();

    /**
     * @brief PowerOff
     * 关机
     */
    void powerOff();

    /*
     * ===============================================
     * 以下为成员变量，包括gsettings等设置相关变量
     * ===============================================
     */
    QGSettings *mPowerManagementGsettings;

    /**
     * @brief mMaxBrightness
     * 最大亮度值
     */
    qulonglong mMaxBrightness;

    /**
     * @brief mBrightness
     * 当前亮度值
     */
    qulonglong mBrightness;

    /**
     * @brief mUserBrightness
     * 用户配置的亮度值
     */
    qulonglong mUserBrightness;

signals:
};

#endif // EVENTWATCHER_H
