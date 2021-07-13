#ifndef DEVICE_WIDGET_H
#define DEVICE_WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QLabel>
#include <QHBoxLayout>
#include "device.h"
#include <QDebug>
#include <QStyleOption>

#define KYLIN_BATTERY_CHARGING0_PATH            ":/charging/battery1/13.svg"
#define KYLIN_BATTERY_CHARGING10_PATH           ":/charging/battery1/13.svg"
#define KYLIN_BATTERY_CHARGING20_PATH           ":/charging/battery1/13.svg"
#define KYLIN_BATTERY_CHARGING30_PATH           ":/charging/battery1/13.svg"
#define KYLIN_BATTERY_CHARGING40_PATH           ":/charging/battery1/13.svg"
#define KYLIN_BATTERY_CHARGING50_PATH           ":/charging/battery1/13.svg"
#define KYLIN_BATTERY_CHARGING60_PATH           ":/charging/battery1/13.svg"
#define KYLIN_BATTERY_CHARGING70_PATH           ":/charging/battery1/13.svg"
#define KYLIN_BATTERY_CHARGING80_PATH           ":/charging/battery1/13.svg"
#define KYLIN_BATTERY_CHARGING90_PATH           ":/charging/battery1/13.svg"
#define KYLIN_BATTERY_CHARGING100_PATH          ":/charging/battery1/13.svg"
#define KYLIN_BATTERY_00_PATH                   ":/charging/battery1/13.svg"

class device_widget : public QWidget
{
    Q_OBJECT
public:
    explicit device_widget(QWidget *parent = nullptr);


    QHBoxLayout *mainLayout;
    QLabel *deviceIconLabel;
    QLabel *deviceNameLabel;
    QLabel *percentageLabel;
    QLabel *batteryLabel;

    QStringList m_IconBatterChangeList;


    void initUI();
    void initDevice(DEVICE *dev);
    void setIcon(QString arg);
    void setBatteryIcon(int Ele_surplus_int);

Q_SIGNALS:

private:
    void paintEvent(QPaintEvent *e);

public  Q_SLOTS:
    void onBatteryChanged(QStringList args);

};

#endif // DEVICE_WIDGET_H
