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


#include "powermanagementdamon.h"

#include <QDebug>

PowerManagementDamon::PowerManagementDamon()
{
    eventwatcher = new EventWatcher;
    eventwatcher->initEventWatcher();
    idlenesswatcher = new IdlenessWatchcer;
    idlenesswatcher->initIdleWatcher();
    lidwatcher = new LidWatcher;
    lidwatcher->initLidWatcher();
    acwatcher = new AcWatcher;
    lowpowerwatcher = new LowPowerWatcher;
    lowpowerwatcher->initLowPowerWatcher();
    gsettingwatcher = new GsettingWatcher;
    gsettingwatcher->initGsettingWatcher();
    powermsgnotificat = new PowerMsgNotificat;
    powermsgnotificat->initPowerMsgNotificat();
}

PowerManagementDamon::~PowerManagementDamon()
{
    delete idlenesswatcher;
    delete lidwatcher;
    delete eventwatcher;
    delete acwatcher;
    delete lowpowerwatcher;
    delete gsettingwatcher;
    delete powermsgnotificat;
}
