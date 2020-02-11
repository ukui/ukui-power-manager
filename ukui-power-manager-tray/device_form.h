#ifndef DEVICE_FORM_H
#define DEVICE_FORM_H

#include <QWidget>
#include "device.h"
#include "enginedevice.h"
#include <QMovie>

namespace Ui {
class DeviceForm;
}

class DeviceForm : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceForm(QWidget *parent = nullptr);
    ~DeviceForm();

public Q_SLOTS:
    void widget_property_change();

    void slider_changed(int value);
    void slot_device_change(DEVICE *device);
public:
    QMovie *mov;
    QString icon_name;
    QString state_text;
    QString percentage;
    int percent_number;
    QString predict;
    DEVICE *m_device;
    EngineDevice *ed;
    QString path;
    void setRemain(QString remain);
    void setState(QString state);
    void setPercent(QString perct);
    void setIcon(QString name);
    QString calculate_value(int nValue, int nTotal);
    void set_device(DEVICE *dev);
protected:
    void leaveEvent(QEvent *event);
    void enterEvent(QEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
private:
    Ui::DeviceForm *ui;
};

#endif // DEVICE_FORM_H
