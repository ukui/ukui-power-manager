TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
TARGET = gpm-backlight-helper
CONFIG += c++11 no_keywords link_pkgconfig
PKGCONFIG +=  gsettings-qt glib-2.0 gio-2.0




SOURCES += \
        main.cpp
