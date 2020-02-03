#ifndef DEVICE_FORM_H
#define DEVICE_FORM_H

#include <QWidget>

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
public:
    QString icon_name;
    QString state_text;
    QString percentage;
    QString predict;
    void setRemain(QString remain);
    void setState(QString state);
    void setPercent(QString perct);
    void setIcon(QString name);
    QString calculate_value(int nValue, int nTotal);
protected:
    void leaveEvent(QEvent *event);
    void enterEvent(QEvent *event);
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private:
    Ui::DeviceForm *ui;
};

#endif // DEVICE_FORM_H
