#ifndef POWERWINDOW_H
#define POWERWINDOW_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QDBusInterface>
#include <QGSettings>
#include <QGuiApplication>
#include <QScreen>
#include <QDBusReply>
#include <QLabel>
#include <QVBoxLayout>
#include "enginedevice.h"
#include <QListWidgetItem>
#include "device_widget.h"
#include <QSlider>
#include <QMouseEvent>
#include <QListWidgetItem>
#include <QMap>
#include <QProcess>
#include <QFont>
#include <QPushButton>

#define KYLIN_BATTERY_CHARGING0_PATH1           ":/charging/battery1/1.svg"
#define KYLIN_BATTERY_CHARGING10_PATH1           ":/charging/battery1/1.svg"
#define KYLIN_BATTERY_CHARGING20_PATH1           ":/charging/battery1/1.svg"
#define KYLIN_BATTERY_CHARGING30_PATH1           ":/charging/battery1/1.svg"
#define KYLIN_BATTERY_CHARGING40_PATH1           ":/charging/battery1/1.svg"
#define KYLIN_BATTERY_CHARGING50_PATH1           ":/charging/battery1/1.svg"
#define KYLIN_BATTERY_CHARGING60_PATH1           ":/charging/battery1/1.svg"
#define KYLIN_BATTERY_CHARGING70_PATH1           ":/charging/battery1/1.svg"
#define KYLIN_BATTERY_CHARGING80_PATH1           ":/charging/battery1/1.svg"
#define KYLIN_BATTERY_CHARGING90_PATH1           ":/charging/battery1/1.svg"
#define KYLIN_BATTERY_CHARGING100_PATH1          ":/charging/battery1/1.svg"
#define KYLIN_BATTERY_00_PATH1                  ":/charging/battery1/1.svg"

class stateslider;
class settinglabel;
class m_PartLineWidget;

class powerwindow : public QWidget
{
    Q_OBJECT
public:
    explicit powerwindow(QWidget *parent = nullptr);

    enum PanelStatePosition
    {
        PanelDown = 0,
        PanelUp,
        PanelLeft,
        PanelRight
    };


    void setWindowProperty();
    void initUI();
    void setPosition();
    void setBatteryIcon(int Ele_surplus_int,int state);
    void initgsetting();
    QStringList m_IconBatterChangeList;

    QVBoxLayout  *m_pmainlayout    = nullptr;
    QHBoxLayout  *m_firstlayout    = nullptr;
    QHBoxLayout  *m_statelayout    = nullptr;
    QHBoxLayout  *lastlayout       = nullptr;
    QWidget      *m_firstwidget;
    QWidget      *m_statewidget;
    QWidget      *lastWidget;

    QLabel *iconLabel;
    QPushButton *iconButton;
    QLabel *percentageLabel;
    QLabel *powerStateLabel;
    QLabel *powerTimeToEmpty;
    stateslider *stateSlider;
    QLabel *enduranceIconLabel;
    QLabel *enduranceLabel;
    QLabel *performanceIconLabel;
    QLabel *performanceLabel;

    m_PartLineWidget *line;

    EngineDevice* ed;
    QListWidget *listWidget;
    QMap<DEVICE*,QListWidgetItem*> device_item_map;
    settinglabel *settingLabel;
    void get_power_list();
Q_SIGNALS:

private :
    QDBusInterface   *m_pServiceInterface;                            // 获取任务栏的高度
    QGSettings       *m_pPanelSetting = nullptr;
    int              m_nScreenWidth;                                  // 屏幕分辨率的宽
    int              m_nScreenHeight;                                 // 屏幕分辨率的高
    int              m_pPeonySite;                                    // 任务栏位置
    int              deviceNum = 0;
    void paintEvent(QPaintEvent *e);
    void initPanelDbusGsetting();
    void listenPanelChange();
    int connectTaskBarDbus();
    int getPanelSite();
    void GetsAvailableAreaScreen();
    int getTaskbarPos(QString str);
    int getTaskbarHeight(QString str);
    int getScreenGeometry(QString methodName);
    void initset_window_position();
    void set_window_position();
private Q_SLOTS:

    void set_preference_func();
    void batteryChange(int dev);
    void onBatteryChanged(QStringList args);

    void add_one_device(DEVICE *device);
    void remove_one_device(DEVICE *device);
    void charge_notify(DEV dev);
    void discharge_notify(DEV dev);
    void action_battery_notify(DEV dev);
    void critical_battery_notify(DEV dev);
    void low_battery_notify(DEV dev);
    void full_charge_notify(DEV dev);
    void IconChanged(QString str);

};


class m_PartLineWidget : public QWidget
{
    Q_OBJECT
public:
    explicit m_PartLineWidget(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event);


};


class stateslider : public QSlider
{
public:
    stateslider(QWidget *parent = 0);
};


class settinglabel : public QLabel
{
    Q_OBJECT
public:
    settinglabel();

protected:
    void mousePressEvent(QMouseEvent *event);

Q_SIGNALS:
    void labelclick();
};

#endif // POWERWINDOW_H
