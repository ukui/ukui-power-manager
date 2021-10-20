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

#ifndef ACWATCHER_H
#define ACWATCHER_H
#include "eventwatcher.h"

class AcWatcher : public EventWatcher
{
    Q_OBJECT
public:
    AcWatcher();
    ~AcWatcher();

    /**
     * @brief initAcWatcher
     * 初始化电源插拔
     */
    void initAcWatcher();

    /**
     * @brief readOnBattery
     * 获取电源状态（AC或者battery）
     * @return
     */
    bool readOnBattery();

    bool &mPowerState = mOnBattery;
signals:
    /**
     * @brief acChanged
     * 电源状态改变信号
     */
    void acChanged(bool);

private slots:
    /**
     * @brief dealAcWatcherMssage
     * 处理电源插拔消息
     */
    void dealAcWatcherMssage(void);

private:
    bool mOnBattery;
};

#endif // ACWATCHER_H
