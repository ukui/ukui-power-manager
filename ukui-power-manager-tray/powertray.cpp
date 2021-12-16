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

#include "powertray.h"
#include "powerwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QProcess>
#include <QDBusInterface>
#include <QLabel>
#include <QPainter>
#include <QFile>
#include <QDir>
#include <QDBusReply>
#include <QThread>
#include <unistd.h>
#include "kwindowsystem.h"

PowerTray::PowerTray(QObject *parent) : QObject(parent)
{
    iface = new QDBusInterface("org.ukui.upower", "/", "org.ukui.upower", QDBusConnection::sessionBus());
    while (!iface->isValid()) {
        QThread::sleep(1);
        iface = new QDBusInterface("org.ukui.upower", "/", "org.ukui.upower", QDBusConnection::sessionBus());
    }
    QDBusPendingReply<QString> reply = iface->call("DaemonVersion");
    reply.waitForFinished();
    if (reply.isValid()) {
        qDebug() << "upower version is:" << reply.isValid() ;
    } else {
        qDebug() << "upower version get error!";
    }
}

void PowerTray::initPowerTray()
{
    trayIcon = new QSystemTrayIcon(this);
    EngineDevice *ed;
    ed = EngineDevice::getInstance();
    QDBusConnection::sessionBus().connect(
        QString(), "/", "org.ukui.upower", "BatteryIcon", this, SLOT(IconChanged(QString)));
    connect(ed, SIGNAL(engine_signal_summary_change(QString)), this, SLOT(onSumChanged(QString)));
    initUi();
    connect(
        trayIcon,
        SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
        this,
        SLOT(onActivatedIcon(QSystemTrayIcon::ActivationReason)));
}

QString PowerTray::getMachineType()
{
    QDBusReply<QString> reply = iface->call("MachineType");
    if (reply.isValid()) {
        if ("book" == reply.value()) {
            qDebug() << "book";
            return "book";
        } else {
            qDebug() << "pc";
            return "pc";
        }
    } else {
        qDebug() << "upower error";
        return "upower error";
    }
}

void PowerTray::onSumChanged(QString str)
{
    trayIcon->setToolTip(str);
}

void PowerTray::IconChanged(QString str)
{
    if (!str.isNull()) {
        QIcon icon = QIcon::fromTheme(str);
        trayIcon->setIcon(icon);
        trayIcon->show();

    } else {
        trayIcon->hide();
    }
}

void PowerTray::onActivatedIcon(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == 3) {
        if (powerWindow->isVisible()) {
            powerWindow->hide();
            return;
        }
        powerWindow->set_window_position();
        powerWindow->show();
        KWindowSystem::setState(powerWindow->winId(), NET::SkipTaskbar | NET::SkipPager);
    }
}

void PowerTray::initUi()
{
    menu = new QMenu();
    menu->setAttribute(Qt::WA_TranslucentBackground);
    trayIcon->setContextMenu(menu);
    QAction *pset_preference = new QAction(menu);
    QIcon icon = QIcon::fromTheme("document-page-setup-symbolic");
    pset_preference->setIcon(icon);
    pset_preference->setText(tr("SetPower"));
    QDBusReply<QString> reply = iface->call("IconName");
    qDebug() << reply.value();
    IconChanged(reply.value());
    connect(pset_preference, &QAction::triggered, this, &PowerTray::set_preference_func);
    menu->addAction(pset_preference);
    powerWindow = new powerwindow();
    powerWindow->hide();
}

void PowerTray::set_preference_func()
{
    QProcess process;
    process.start(
        "bash",
        QStringList() << "-c"
                      << "dpkg -l | grep ukui-control-center");
    process.waitForFinished();
    QString strResult = process.readAllStandardOutput() + process.readAllStandardError();
    if (-1 != strResult.indexOf("3.0")) {
        QProcess::startDetached(QString("ukui-control-center -p"));
    } else {
        QProcess::startDetached(QString("ukui-control-center -m Power"));
    }
}

PowerTray::~PowerTray()
{
    delete powerWindow;
    delete menu;
    delete trayIcon;
    delete iface;
}
