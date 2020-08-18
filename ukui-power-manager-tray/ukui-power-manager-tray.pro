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
TRANSLATIONS+=\
    translations/ukui-power-manager-tray_bo.ts \
    translations/ukui-power-manager-tray_zh_CN.ts \
    translations/ukui-power-manager-tray_tr.ts

QM_FILES_INSTALL_PATH = /usr/share/ukui-power-manager/tray/translations/

# CONFIG += lrelase not work for qt5.6, add those from lrelease.prf for compatibility
qtPrepareTool(QMAKE_LRELEASE, lrelease)
lrelease.name = lrelease
lrelease.input = TRANSLATIONS
lrelease.output = ${QMAKE_FILE_IN_BASE}.qm
lrelease.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
lrelease.CONFIG = no_link
QMAKE_EXTRA_COMPILERS += lrelease
PRE_TARGETDEPS += compiler_lrelease_make_all

for (translation, TRANSLATIONS) {
    translation = $$basename(translation)
    QM_FILES += $$OUT_PWD/$$replace(translation, \\..*$, .qm)
}
qm_files.files = $$QM_FILES
qm_files.path = $$QM_FILES_INSTALL_PATH
qm_files.CONFIG = no_check_exist
INSTALLS += qm_files

# So we can access it from main.cpp
DEFINES += QM_FILES_INSTALL_PATH='\\"$${QM_FILES_INSTALL_PATH}\\"'


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
