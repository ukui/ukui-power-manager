#-------------------------------------------------
#
# Project created by QtCreator 2020-01-06T10:55:07
#
#-------------------------------------------------

QT       += core gui dbus

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
TRANSLATIONS += zh_CN.ts en.ts

CONFIG += c++11 no_keywords link_pkgconfig
PKGCONFIG +=  gsettings-qt

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    device_widget.cpp \
    customtype.cpp \
    device.cpp \
    engine-common.cpp \
    enginedevice.cpp \
    device_form.cpp

HEADERS += \
        mainwindow.h \
    device_widget.h \
    customtype.h \
    device.h \
    engine-common.h \
    enginedevice.h \
    device_form.h

FORMS += \
        mainwindow.ui \
    devicewidget.ui \
    deviceform.ui

# Default rules for deployment.

RESOURCES += \
    resource.qrc
