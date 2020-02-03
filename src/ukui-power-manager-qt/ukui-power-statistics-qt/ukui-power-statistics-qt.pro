#-------------------------------------------------
#
# Project created by QtCreator 2019-10-17T09:33:29
#
#-------------------------------------------------

QT       += core gui dbus charts
#QT       += x11extras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ukui-power-statistics-qt
TEMPLATE = app

target.path = /usr/bin
INSTALLS += target

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
TRANSLATIONS += zh_CN.ts en.ts

#LIBS += -lX11
PKGCONFIG += gio-2.0
CONFIG += link_pkgconfig

SOURCES += \
    customtype.cpp \
    eggunique.cpp \
    gsettings.cpp \
    main.cpp \
    ukpm-widget.cpp \
    titlewidget.cpp \
    device.cpp

HEADERS += \
    customtype.h \
    eggunique.h \
    gsettings.h \
    statistics-common.h \
    ukpm-widget.h \
    titlewidget.h \
    device.h

SUBDIRS += \
    ukui-power-statistics-qt.pro

DISTFILES += \
    zh_CN.qm \
    ukui-power-statistics-qt.pro.user \
    zh_CN.ts

RESOURCES += \
    resource.qrc
