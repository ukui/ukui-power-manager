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
#include <QApplication>
#include <QFile>
#include <QSharedMemory>
#include <QDebug>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSharedMemory mem("SingleApp-ukui-power-manager-tray");
    if(mem.attach())
    {
        mem.detach();
    }
    QSharedMemory unimem("SingleApp-ukui-power-manager-tray");
    bool is_run=false;
    if(unimem.attach())
    {
        is_run = true;
    }
    else
    {
        unimem.create(1);
        is_run = false;
    }
    if(is_run)
    {
        qWarning()<<QStringLiteral("program is already running! exit!");
        exit(0);
    }
    QString locale = QLocale::system().name();
    QTranslator translator;
    QString qmfile = QString(":/%1.qm").arg(locale);
    qDebug()<<qmfile;
    translator.load(qmfile);
    a.installTranslator(&translator);
    QFile file(":/main.qss");
    file.open(QFile::ReadOnly);
    qApp->setStyleSheet(file.readAll());
    file.close();
    QIcon::setThemeName("ukui-icon-theme-default");
    MainWindow w;
    w.hide();

    return a.exec();
}
