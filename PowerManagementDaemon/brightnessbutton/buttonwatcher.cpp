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


#include "buttonwatcher.h"

ButtonWatcher::ButtonWatcher() {}

ButtonWatcher::~ButtonWatcher() {}

void ButtonWatcher::initButtonWatcher()
{
    QDBusConnection::sessionBus().connect(
        QString(),
        SETTINGS_DAEMON_MEDIAKEYS_PATH,
        SETTINGS_DAEMON_MEDIAKEYS,
        "brightnessDown",
        this,
        SLOT(dealBrightnessDown()));

    QDBusConnection::sessionBus().connect(
        QString(),
        SETTINGS_DAEMON_MEDIAKEYS_PATH,
        SETTINGS_DAEMON_MEDIAKEYS,
        "brightnessUp",
        this,
        SLOT(dealBrightnessUp()));
}

void ButtonWatcher::dealBrightnessDown()
{
    emit brightnessDown();
}

void ButtonWatcher::dealBrightnessUp()
{
    emit brightnessUp();
}
