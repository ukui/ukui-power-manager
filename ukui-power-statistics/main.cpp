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
#include <QApplication>
#include <QTranslator>
#include <eggunique.h>
#include "ukpm_widget.h"
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <X11/Xlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    int fd = open (QString(homePath.at(0) + "/.config/ukui-power-statistics%1.lock").arg(getenv("DISPLAY")).toUtf8().data(),O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
    if(fd < 0)  exit(1);
    if(lockf(fd,F_TLOCK,0))
    {
        qDebug()<<"cant lock single file, ukui-power-statistics is already running";
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

    if (width > 2560) {
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
                QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
                QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
        #endif
    }

    XCloseDisplay(display);

    QApplication a(argc, argv);
    QCommandLineParser parser;
    QCommandLineOption op("device","device","device","");
    parser.addOption(op);
    parser.parse(a.arguments());
    QString device = parser.value(op);

    QTranslator translator;
    if (translator.load(QLocale(),"ukui-power-statistics","_",QM_FILES_INSTALL_PATH))
        a.installTranslator(&translator);
    else
        qDebug()<<"load ukui-power-manager-statistics qm file error";

    QIcon icon = QIcon::fromTheme("ukui-power-statistics");
    a.setWindowIcon(icon);
    UkpmWidget *w = new UkpmWidget;
    if(!device.isEmpty())
    {
        w->set_selected_device(device);
    }

    w->show();
    return a.exec();
}
