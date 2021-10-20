QT       += core gui dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = ukui-powermanagement

CONFIG += c++11 link_pkgconfig

PKGCONFIG += gsettings-qt

LIBS += -lukui-log4qt

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    acwatcher/acwatcher.cpp \
    brightnessbutton/buttonwatcher.cpp \
    eventwatcher.cpp \
    gsettingwatcher/gsettingwatcher.cpp \
    lowpowerwatcher/lowpowerwatcher.cpp \
    main.cpp \
    powermanagementdamon.cpp \
    idle/idlenesswatcher.cpp \
    lidwatcher/lidwatcher.cpp \
    powermsgnotificat/powermsgnotificat.cpp \
    powerwatcher/powerwatcher.cpp

HEADERS += \
    acwatcher/acwatcher.h \
    brightnessbutton/buttonwatcher.h \
    eventwatcher.h \
    gsettingwatcher/gsettingwatcher.h \
    lowpowerwatcher/lowpowerwatcher.h \
    powermanagementdamon.h \
    idle/idlenesswatcher.h \
    lidwatcher/lidwatcher.h \
    common.h \
    powermsgnotificat/powermsgnotificat.h \
    powerwatcher//powerwatcher.h

# Default rules for deployment.
qnx: target.path = /tmp/usr/bin
else: unix:!android: target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target

desktop.files += resources/ukui-powermanagement-daemon.desktop

desktop.path = /etc/xdg/autostart/

INSTALLS += desktop
