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
#include <ukui-log4qt.h>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    int fd = open(
        QString(homePath.at(0) + "/.config/ukui-power-manager-tray%1.lock").arg(getenv("DISPLAY")).toUtf8().data(),
        O_WRONLY | O_CREAT | O_TRUNC,
        S_IRUSR | S_IWUSR);
    if (fd < 0) {
        exit(1);
    }
    if (lockf(fd, F_TLOCK, 0)) {
        qDebug() << "cant lock single file, ukui-power-manager-tray is already running";
        exit(0);
    }
    QApplication app(argc, argv);
    initUkuiLog4qt("ukui-power-manager-tray");
    QTranslator translator;
    if (translator.load(QLocale(), "ukui-power-manager-tray", "_", QM_FILES_INSTALL_PATH)) {
        app.installTranslator(&translator);
    } else {
        qDebug() << "load ukui-power-manager-tray qm file error";
    }

//    QTime dieTime = QTime::currentTime().addMSecs(2000);
//    while (QTime::currentTime() < dieTime) {
//        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
//    }

    PowerTray p;
    if ("book" == p.getMachineType()) {
        p.initPowerTray();
    } else {
        qDebug() << "No tray required";
        exit(0);
    }
    return app.exec();
}
