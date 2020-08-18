#-------------------------------------------------
#
# Project created by QtCreator 2019-10-17T09:33:29
#
#-------------------------------------------------

QT       += core gui dbus charts
#QT       += x11extras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ukui-power-statistics
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
TRANSLATIONS+=\
    translations/ukui-power-statistics_bo.ts \
    translations/ukui-power-statistics_zh_CN.ts \
    translations/ukui-power-statistics_tr.ts

QM_FILES_INSTALL_PATH = /usr/share/ukui-power-manager/statistics/translations/

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


CONFIG += c++11 link_pkgconfig
PKGCONFIG += gsettings-qt x11

SOURCES += \
    customtype.cpp \
    eggunique.cpp \
    main.cpp \
    titlewidget.cpp \
    device.cpp \
    ukpm_widget.cpp

HEADERS += \
    customtype.h \
    eggunique.h \
    statistics_common.h \
    titlewidget.h \
    device.h \
    ukpm_widget.h

RESOURCES += \
    resource.qrc
