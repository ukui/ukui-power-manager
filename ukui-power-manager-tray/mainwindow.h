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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGSettings>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QScrollArea>
#include <QDBusObjectPath>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QMap>
#include <QListWidgetItem>
#include <QWidgetAction>
#include <QLabel>
#include <QTimer>
#include <powerwindow.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initUi();

private:
    powerwindow *powerWindow;
    QSystemTrayIcon* trayIcon;
    QList<QDBusObjectPath> deviceNames;
    QGSettings * settings;
    QMenu *menu;

private Q_SLOTS:
    void set_preference_func();
    void onSumChanged(QString str);
    void IconChanged(QString str);
    void onActivatedIcon(QSystemTrayIcon::ActivationReason reason);

};

#endif // MAINWINDOW_H
