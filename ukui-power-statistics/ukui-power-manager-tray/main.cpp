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
#include <QStandardPaths>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    #if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
        QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    #endif
    QStringList homePath =
	QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    int fd =
	open(QString
	     (homePath.at(0) +
	      "/.config/ukui-power-manager-tray%1.lock").
	     arg(getenv("DISPLAY")).toUtf8().data(),
	     O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0)
	exit(1);
    if (lockf(fd, F_TLOCK, 0)) {
	qDebug() <<
	    "cant lock single file, ukui-power-manager-tray is already running";
	exit(0);
    }

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


    XCloseDisplay(display);

    QApplication a(argc, argv);
    QTranslator translator;
    if (translator.
	load(QLocale(), "ukui-power-manager-tray", "_",
	     QM_FILES_INSTALL_PATH)) {
	a.installTranslator(&translator);
    } else
	qDebug() << "load ukui-power-manager-tray qm file error";

    QFile file(":/main.qss");
    file.open(QFile::ReadOnly);
//    QIcon::setThemeName("ukui-icon-theme-default");
    qApp->setStyleSheet(file.readAll());
    file.close();
    MainWindow w;
    KWindowEffects::enableBlurBehind(w.winId(), true);
    w.hide();

    return a.exec();
}
