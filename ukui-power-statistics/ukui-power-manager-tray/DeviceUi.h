#ifndef DEVICEUI_H
#define DEVICEUI_H
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE class DeviceUi {
  public:
    QLabel * icon;
    QLabel *kind;
    QLabel *percentage;
    QLabel *remaintext;
    QLabel *remaindata;
    QProgressBar *progressBar;
    void uiInit(QWidget * DeviceForm);
};


#endif				// DEVICEUI_H
