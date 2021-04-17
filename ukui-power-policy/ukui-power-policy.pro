QT += core dbus
QT -= gui

CONFIG += c++11

TARGET = ukui-power-policy
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    powerpolicy.cpp \
    gsettings.cpp \
    policy_config.cpp \
    huawei.cpp

target.path = /usr/bin
INSTALLS += target
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += /usr/include/libdrm
LIBS += -ldrm
PKGCONFIG += gio-2.0
CONFIG += link_pkgconfig
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DBUS_ADAPTORS += ukui.power.policy.xml
DISTFILES +=

HEADERS += \
    powerpolicy.h \
    dbus_struct.h \
    gsettings.h \
    policy_config.h \
    huawei.h
