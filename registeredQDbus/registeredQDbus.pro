QT       += core dbus x11extras
QT       -= gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ukui-powermanagement-service
TEMPLATE = app

CONFIG += console c++11 link_pkgconfig
CONFIG -= app_bundle

DESTDIR = .
INCLUDEPATH += .
DEFINES += MODULE_NAME=\\\"pm\\\"
inst1.files += conf/org.ukui.powermanagement.service
inst1.path = /usr/share/dbus-1/system-services/
inst2.files += conf/org.ukui.powermanagement.conf
inst2.path = /etc/dbus-1/system.d/

target.source += $$TARGET
target.path = /usr/bin
INSTALLS += \
    target \
    inst1 \
    inst2 \

HEADERS += \
    sysdbusregister.h \


SOURCES += \
    main.cpp \
    sysdbusregister.cpp \

