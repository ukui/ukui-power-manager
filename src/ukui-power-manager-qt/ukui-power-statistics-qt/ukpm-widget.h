/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */
#ifndef UKPM_WIDGET_H
#define UKPM_WIDGET_H

#include "titlewidget.h"
#include <math.h>
#include <QTimer>
#include <QMap>
#include <QList>
#include <QProcess>
#include <QDebug>
#include <QTime>
#include <QDateTime>
#include <QToolButton>
#include <QWidget>
#include <QListWidget>
#include <QTabWidget>
#include <QStackedWidget>
#include <QTableWidget>
#include <QSplitter>
#include "QLabel"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QSpacerItem>
#include <QChart>
#include <QLineSeries>
#include <QChartView>
#include <QPainter>
#include <QValueAxis>
#include <QTableWidget>
#include <QHeaderView>
#include <QIcon>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QDesktopWidget>
#include <QApplication>
#include <QSplineSeries>
#include <QDesktopServices>
#include <QDBusConnection>
#include <QDBusArgument>
#include <QDBusInterface>
#include <QDBusReply>
#include <QFormLayout>
#include <QWidgetAction>
#include "gsettings.h"
#include <QCategoryAxis>
#include "customtype.h"
#include "statistics-common.h"
#include "device.h"


#define WORKING_DIRECTORY "."
#define DBUS_SERVICE "org.freedesktop.UPower"
#define DBUS_OBJECT "/org/freedesktop/UPower"
#define DBUS_INTERFACE "org.freedesktop.DBus.Properties"
#define DBUS_INTERFACE_PARAM "org.freedesktop.UPower.Device"

#include <QStandardItemModel>
#include <QTableView>

enum SUMTYPE
{
    CHARGE,CHARGE_ACCURENCY,DISCHARGING,DISCHARGING_ACCURENCY
};

enum HISTYPE
{
    RATE,VOLUME,CHARGE_DURATION,DISCHARGING_DURATION
};

enum TIMESPAN
{
    TENM,TWOH,SIXH,ONED,ONEW
};

QT_CHARTS_USE_NAMESPACE
class UkpmWidget : public QWidget
{
    Q_OBJECT

public:
    UkpmWidget(QWidget *parent = 0);
    ~UkpmWidget();
    void setUI();
    void setHistoryTab();
    void setSumTab();
    void connectSlots();
    QList<QPointF> setdata(); //设置图表数据的函数接口
    void getDcDetail();
    void getBtrDetail();
    void getAll(DEV *dc);
    void calcTime(QString &attr, uint time);
    void getDevices();
    void setupDcUI();
    void setupBtrUI();
    void initUI();
    void addNewUI(QDBusObjectPath &path, UpDeviceKind newKind);
    int parseArguments();
    QString getWidgetAxis(uint value);
    void setupBtrDetail();
    QString device_kind_to_localised_text(UpDeviceKind kind, uint number);

    QString getSufix(uint tim, char c)
    {
        QString strValue;
        if('s'==c)
            strValue = QString::number(tim) + " " + tr("s");
        else if('m'==c)
            strValue = QString::number(tim) + " " + tr("m");
        else if('h'==c)
            strValue = QString::number(tim,'f', 1) + " " + tr("h");
        return strValue;
    }
    QString boolToString(bool ret)
    {
        return ret ? tr("yes") : tr("no");
    }
    QString up_device_kind_to_string (UpDeviceKind type_enum)
    {
        switch (type_enum) {
        case UP_DEVICE_KIND_LINE_POWER:
            return ("line-power");
        case UP_DEVICE_KIND_BATTERY:
            return ("battery");
        case UP_DEVICE_KIND_UPS:
            return ("ups");
        case UP_DEVICE_KIND_MONITOR:
            return ("monitor");
        case UP_DEVICE_KIND_MOUSE:
            return ("mouse");
        case UP_DEVICE_KIND_KEYBOARD:
            return ("keyboard");
        case UP_DEVICE_KIND_PDA:
            return ("pda");
        case UP_DEVICE_KIND_PHONE:
            return ("phone");
        case UP_DEVICE_KIND_MEDIA_PLAYER:
            return ("media-player");
        case UP_DEVICE_KIND_TABLET:
            return ("tablet");
        case UP_DEVICE_KIND_COMPUTER:
            return ("computer");
        default:
            return ("unknown");
        }
    }

    void ukpm_update_info_data_page(DEV *device, int page);
    void ukpm_update_info_page_stats(DEV *device);
    void ukpm_update_info_page_details(DEV *device);
    void ukpm_update_info_data(DEV *device);
    void ukpm_update_info_page_history(DEV *device);
    void getSlots();
    void draw_history_graph(QList<QPointF> list);
    void draw_stats_graph(QList<QPointF> list);
    void addListRow(QString attr, QString value);
    void setInfoUI();
    QList<QPointF> getHistory(QString type, uint timeSpan);
    void ukpm_set_graph_data(QList<QPointF> list, bool use_smoothed, bool use_points);
    QList<QPointF> getStatics(QString stat_type);
    void getProperty(QString path, DEV &dev);
    void setupUI();
    void setDetailTab();

public Q_SLOTS:
    void onActivatedIcon(QSystemTrayIcon::ActivationReason reason);
    void onShow();
    void showHisDataPoint(bool flag);
    void showSumDataPoint(bool flag);
    void onHelpButtonClicked();
    void onExitButtonClicked();
    void drawSumSpineline(bool flag);
    void drawHisSpineline(bool flag);
    void helpFormat();
    void control_center_power();
    void deviceAdded(QDBusMessage msg);
    void deviceRemoved(QDBusMessage msg);
    void onItemChanged(QListWidgetItem *, QListWidgetItem *);
    void onPageChanged(int index);
    void upHistoryType(int index);
    void upStatsType(int index);
    void devPropertiesChanged(QString object_path);

public:
    uint timeSpan, resolution;
    QListWidget *listWidget;
    QStackedWidget *stackedWidget, *hisStack, *sumStack;

    HISTYPE mHISTYPE;
    SUMTYPE mSUMTYPE;
    QChart *hisChart;
    QChartView *hisChartView;
    QLineSeries *hisSeries;
    QSplineSeries *hisSpline;
    QCategoryAxis *axisY;
    QCategoryAxis *xtime;

    QValueAxis *x, *y;
    QChart *sumChart;
    QChartView *sumChartView;
    QLineSeries *sumSeries;
    QSplineSeries *sumSpline;
    QComboBox *sumTypeCombox;
    QComboBox *spanCombox ;
    QComboBox *typeCombox;

    QMenu *menu;
    QCheckBox *hisDataBox, *sumDataBox;
    QCheckBox *hisCurveBox, *sumCurveBox;

    QList<QDBusObjectPath> deviceNames;
    QList<DEVICE*> devices;
    QMap<QDBusObjectPath,QListWidgetItem*> listItem;
//    QMap<DEVICE*,QListWidgetItem*> dev_item;
    QMap<QListWidgetItem*,DEVICE*> dev_item;
    QMap<QDBusObjectPath,QTabWidget*> widgetItem;
    QString batterySvr,acSvr;
    bool iconflag;
    QGSettings *settings;
    QTimer timer;
    TitleWidget *title;
    QColor plotcolor;
    QComboBox *histype;
    QWidget *detail_widget;
    QWidget *his_widget;
    QWidget *stat_widget;
    QTabWidget *tab_widget;
    bool checked,points;
    QComboBox *stat_type;
    DEV *current_device;
    QStandardItemModel* model;
    QTableView *tableView;
    int index_old;
    void ukpm_set_choice_sum();
    void ukpm_set_choice_history();
};

#endif // UKPM_WIDGET_H
