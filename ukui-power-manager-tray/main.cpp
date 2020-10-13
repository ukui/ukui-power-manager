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
#include <KWindowEffects>
#include <QDesktopWidget>
#include <X11/Xlib.h>
#include <QFileInfo>

int main(int argc, char *argv[])
{
//    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    Display *display = XOpenDisplay(NULL);
    if (NULL == display) {
	qDebug() << "Can't open display!";
	return -1;
    }
    Screen *screen = DefaultScreenOfDisplay(display);
    if (NULL == screen) {
	qDebug() << "Get default screen failed!";
        return -1;
    }
    int width = screen->width;

    if (width > 2560) {
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
                QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
                QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
        #endif
    }

    XCloseDisplay(display);

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

//    QString locale = QLocale::system().name();
//    QTranslator translator;
//    QString qmfile = QString("/usr/share/ukui-power-manager/tray/translations/%1.qm").arg(locale);
//    QFileInfo fl(qmfile);
//    if (fl.exists()) {
//        translator.load(qmfile);
//        a.installTranslator(&translator);
//    }
    QTranslator translator;
    if (translator.load(QLocale(),"ukui-power-manager-tray","_",QM_FILES_INSTALL_PATH))
        a.installTranslator(&translator);
    else
        qDebug()<<"load ukui-power-manager-tray qm file error";

    QFile file(":/main.qss");
    file.open(QFile::ReadOnly);
    qApp->setStyleSheet(file.readAll());
    file.close();
//    QIcon::setThemeName("ukui-icon-theme");
    MainWindow w;
    KWindowEffects::enableBlurBehind(w.winId(),true);
    w.hide();

    return a.exec();
}
