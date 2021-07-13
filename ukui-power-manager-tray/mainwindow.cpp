/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
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
#include "mainwindow.h"
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
#include <customstyle.h>
#include <QDBusReply>
#include <unistd.h>
#define PANEL_DBUS_SERVICE          "com.ukui.panel.desktop"
#define PANEL_DBUS_PATH             "/"
#define PANEL_DBUS_INTERFACE        "com.ukui.panel.desktop"
#define PANEL_SETTINGS              "org.ukui.panel.settings"
#define PANEL_SETTINGS_KEY_HEIGHT   "panelsize"
#define PANEL_SETTINGS_KEY_POSITION "panelposition"

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent)
{
    trayIcon = new QSystemTrayIcon(this);
    //trayIcon->show();
    EngineDevice* ed;
    ed = EngineDevice::getInstance();
    connect(ed, SIGNAL(icon_changed(QString)), this,SLOT(IconChanged(QString)));
    connect(ed, SIGNAL(engine_signal_summary_change(QString)), this,SLOT(onSumChanged(QString)));
    initUi();
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(onActivatedIcon(QSystemTrayIcon::ActivationReason)));

}
void MainWindow::onSumChanged(QString str){
	trayIcon->setToolTip(str);
}
void MainWindow::IconChanged(QString str){
    //qDebug()<<"::::::::"<<str;
    if (!str.isNull()) {
        QIcon icon = QIcon::fromTheme(str);
        trayIcon->setIcon(icon);
        trayIcon->show();

    } else {
        trayIcon->hide();
    }

}
void MainWindow::onActivatedIcon(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == 3){
        powerWindow->show();
    }
}

void MainWindow::initUi()
{
    setWindowFlags(Qt::FramelessWindowHint|Qt::Popup);
    setAttribute(Qt::WA_StyledBackground,true);
    setAttribute(Qt::WA_TranslucentBackground);

    menu = new QMenu(this);
    menu->setAttribute(Qt::WA_TranslucentBackground);
    trayIcon->setContextMenu(menu);
    QAction *pset_preference  = new QAction(menu);
    QIcon icon = QIcon::fromTheme("document-page-setup-symbolic");
    pset_preference->setIcon(icon);
    pset_preference->setText(tr("SetPower"));
    connect(pset_preference,&QAction::triggered,this,&MainWindow::set_preference_func);
    menu->addAction(pset_preference);
    powerWindow = new powerwindow();
    powerWindow->hide();
}

void MainWindow::set_preference_func()
{
    QProcess::startDetached(QString("ukui-control-center -p &"));
}


MainWindow::~MainWindow()
{

}

