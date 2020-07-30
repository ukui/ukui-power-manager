#-------------------------------------------------
#
# Project created by QtCreator 2020-01-06T10:55:07
#
#-------------------------------------------------

QT       += core gui dbus KWindowSystem

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ukui-power-manager-tray
TEMPLATE = app

target.path = /usr/bin
INSTALLS += target

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
TRANSLATIONS += zh_CN.ts en.ts bo.ts

CONFIG += c++11 no_keywords link_pkgconfig
PKGCONFIG +=  gsettings-qt x11

SOURCES += \
    customstyle.cpp \
        main.cpp \
        mainwindow.cpp \
    customtype.cpp \
    device.cpp \
    enginedevice.cpp \
    device_form.cpp

HEADERS += \
    customstyle.h \
    engine_common.h \
        mainwindow.h \
    customtype.h \
    device.h \
    enginedevice.h \
    device_form.h

FORMS += \
        mainwindow.ui \
    deviceform.ui

# Default rules for deployment.

RESOURCES += \
    resource.qrc
