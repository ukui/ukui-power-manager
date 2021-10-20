#-------------------------------------------------
#
# Project created by QtCreator 2019-06-29T11:18:10
#
#-------------------------------------------------

QT       += core gui dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ukui-upower
TEMPLATE = app

CONFIG += c++11 link_pkgconfig
PKGCONFIG += gsettings-qt

SOURCES += main.cpp\
    batteryinfo/batteryinfo.cpp \
    upowerinfo/upowerinfo.cpp \
        widget.cpp \
    dbus.cpp \
    dbus-ukuipower.cpp

HEADERS  += widget.h \
    batteryinfo/batteryinfo.h \
    common/common.h \
    dbus.h \
    dbus-ukuipower.h \
    upowerinfo/upowerinfo.h

FORMS    += widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/usr/bin
else: unix:!android: target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target

desktop.files += resources/ukui-upower.desktop
desktop.path = /etc/xdg/autostart/
INSTALLS += desktop


xorg.files += resources/10-monitor.conf
xorg.path = /usr/share/X11/xorg.conf.d/
INSTALLS += xorg

conf.files += resources/org.ukui.power-manager.gschema.xml
conf.path=/usr/share/glib-2.0/schemas/
INSTALLS += conf
