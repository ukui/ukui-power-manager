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


#ifndef POWERMSGNOTIFICAT_H
#define POWERMSGNOTIFICAT_H

#include <QDBusConnection>
#include <QDBusInterface>
#include <QObject>

#include "eventwatcher.h"

class PowerMsgNotificat : public EventWatcher
{
    Q_OBJECT
public:
    PowerMsgNotificat();
    ~PowerMsgNotificat();
    void initPowerMsgNotificat();

private:
    void notifySend(const QString &type, const QString &arg);

public slots:
    void msgNotification(QString);
    //    void dealLowBatteryNotify(bool);
};

#endif // POWERMSGNOTIFICAT_H
