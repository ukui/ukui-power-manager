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


#ifndef POWERMANAGEMENTDAMON_H
#define POWERMANAGEMENTDAMON_H

#include "idle/idlenesswatcher.h"
#include "lidwatcher/lidwatcher.h"
#include "acwatcher/acwatcher.h"
#include "eventwatcher.h"
#include "gsettingwatcher/gsettingwatcher.h"
#include "lowpowerwatcher/lowpowerwatcher.h"
#include "powermsgnotificat/powermsgnotificat.h"

class PowerManagementDamon : public QObject
{
    Q_OBJECT

public:
    PowerManagementDamon();
    ~PowerManagementDamon();

private:
    IdlenessWatchcer *idlenesswatcher;
    LidWatcher *lidwatcher;
    EventWatcher *eventwatcher;
    AcWatcher *acwatcher;
    GsettingWatcher *gsettingwatcher;
    LowPowerWatcher *lowpowerwatcher;
    PowerMsgNotificat *powermsgnotificat;
};
#endif // POWERMANAGEMENTDAMON_H
