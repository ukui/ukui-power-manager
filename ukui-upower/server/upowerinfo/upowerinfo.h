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

#ifndef UPOWERINFO_H
#define UPOWERINFO_H

#include <QObject>
#include <QtDBus>
#include <QDBusInterface>
#include "../common/common.h"

class UPowerInfo : public QObject
{
    Q_OBJECT
public:
    explicit UPowerInfo(QObject *parent = nullptr);

    ~UPowerInfo();
    /**
     * @brief UPowerVersion
     * @return
     * UPower版本
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

    bool OnBattery();

    bool LidIsClosed();

    QString DaemonVersion();

private slots:

    void dealMessage(void);
private:
    void initUpowerInfo();

    void dealPowerMessage(void);

    void dealLidMessage(void);

    QDBusInterface *iface;

    bool mOnBattery;

    bool mLidIsClosed;

signals:
    /**
     * @brief acChanged
     * 电源状态改变信号
     */
    void acChanged(bool );

    /**
     * @brief acChanged
     * 合盖状态改变信号
     */
    void lidChanged(bool );
};

#endif // UPOWERINFO_H
