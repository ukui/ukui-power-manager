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
#include <QCategoryAxis>
#include "customtype.h"
#include "statistics_common.h"
#include "device.h"
#include <QButtonGroup>
#include <QScatterSeries>
#include <QStandardItemModel>
#include <QTableView>
#include <QGSettings>

#define WORKING_DIRECTORY "."
#define DBUS_SERVICE "org.freedesktop.UPower"
#define DBUS_OBJECT "/org/freedesktop/UPower"
#define DBUS_INTERFACE "org.freedesktop.DBus.Properties"
#define DBUS_INTERFACE_PARAM "org.freedesktop.UPower.Device"

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
    void setHistoryTab();
    void setSumTab();
    QList<QPointF> setdata(); //设置图表数据的函数接口
    void getAll(DEV *dc);
    void calcTime(QString &attr, uint time);
    void getDevices();
    int parseArguments();
    QString getWidgetAxis(uint value);
    QString device_kind_to_localised_text(UpDeviceKind kind, uint number);
    QString getSufix(uint tim, char c);
    QString boolToString(bool ret){ return ret ? tr("yes") : tr("no"); }
    QString up_device_kind_to_string (UpDeviceKind type_enum);
    void ukpm_update_info_data_page(DEV *device, int page);
    void ukpm_update_info_page_stats(DEV *device);
    void ukpm_update_info_page_details(DEV *device);
    void ukpm_update_info_data(DEV *device);
    void ukpm_update_info_page_history(DEV *device);
    void getSlots();
    void draw_history_graph(QString type);
    void draw_stats_graph(QString type);
    void addListRow(QString attr, QString value);
    QList<QPointF> getHistory(QString type, uint timeSpan);
    QList<QPointF> getStatics(QString stat_type);
    void getProperty(QString path, DEV &dev);
    void setupUI();
    void setDetailTab();
    bool set_selected_device(QString name);
    int calculate_up_number(float value, int div);
    int calculate_down_number(float value, int div);
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
    void ukpm_set_choice_sum();
    void ukpm_set_choice_history();
    void onitemSelectionChanged();
    void choose_history_graph(int choice);
    void choose_stat_graph(int choice);

public:
    uint timeSpan, resolution;
    QListWidget *listWidget;
    QStackedWidget *stackedWidget;// *hisStack, *sumStack;
    QChart *hisChart;
    QChartView *hisChartView;
    QLineSeries *hisSeries;
    QScatterSeries *hisSpline;
    QCategoryAxis *axisY;
    QCategoryAxis *xtime;

    QValueAxis *x, *y;
    QChart *sumChart;
    QChartView *sumChartView;
    QLineSeries *sumSeries;
    QScatterSeries *sumSpline;
    QComboBox *sumTypeCombox;
    QComboBox *spanCombox ;
    QComboBox *typeCombox;

    QMenu *menu;
    QCheckBox *hisDataBox, *sumDataBox;
    QCheckBox *hisCurveBox, *sumCurveBox;

    QPushButton *his_data_btn, *sum_data_btn;
    QPushButton *his_line_btn, *sum_line_btn;
    QButtonGroup *his_group;
    QButtonGroup *sum_group;

    QList<QDBusObjectPath> deviceNames;
    QList<DEVICE*> devices;
    QMap<QDBusObjectPath,QListWidgetItem*> listItem;
    QMap<QListWidgetItem*,DEVICE*> dev_item;
    QString batterySvr,acSvr;
    QGSettings *settings;
    TitleWidget *title;
    QColor plotcolor;
    QComboBox *histype;
    QWidget *detail_widget;
    QWidget *his_widget;
    QWidget *stat_widget;
    QTabWidget *tab_widget;
    QComboBox *stat_type;
    DEV *current_device;
    QStandardItemModel* model;
    QTableView *tableView;
    int index_old;
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // UKPM_WIDGET_H
