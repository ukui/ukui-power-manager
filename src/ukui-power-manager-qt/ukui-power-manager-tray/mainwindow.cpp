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
#include "mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QProcess>
#include "ui_mainwindow.h"
#include <QDBusInterface>
#include "device_form.h"
#include <QLabel>

#define POWER_SCHEMA "org.ukui.power-manager"
#define POWER_SCHEMA_KEY "power-manager"
//#define DEBUG

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ed = EngineDevice::getInstance();
    ui->setupUi(this);
    initData();
//    setting = new QGSettings(POWER_SCHEMA);
//    connect(setting,SIGNAL(changed(const QString &)),this,SLOT(iconThemeChanged()));

    trayIcon = new QSystemTrayIcon(this);
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(onActivatedIcon(QSystemTrayIcon::ActivationReason)));
    connect(ed,SIGNAL(icon_changed(QString)),this,SLOT(onIconChanged(QString)));
    connect(ed,SIGNAL(engine_signal_summary_change(QString)),this,SLOT(onSumChanged(QString)));
    connect(ed,SIGNAL(engine_signal_charge_low(DEV)),this,SLOT(low_battery_notify(DEV)));
    connect(ed,SIGNAL(engine_signal_charge_critical(DEV)),this,SLOT(critical_battery_notify(DEV)));
    connect(ed,SIGNAL(engine_signal_charge_action(DEV)),this,SLOT(action_battery_notify(DEV)));
    connect(ed,SIGNAL(engine_signal_discharge(DEV)),this,SLOT(discharge_notify(DEV)));
    connect(ed,SIGNAL(engine_signal_fullycharge(DEV)),this,SLOT(full_charge_notify(DEV)));

    setObjectName("MainWindow");
    initUi2();
    disp_control = true;
    menu = new QMenu(this);
    set_preference  = new QAction(menu);
    show_percentage = new QAction(menu);
    set_bright = new QAction(menu);
    set_preference->setIcon(QIcon(":/22x22/apps/setting.svg"));
    set_preference->setText("SetPowerSleep");
    show_percentage->setIcon(QIcon(":/22x22/apps/tick.png"));
    show_percentage->setText("ShowPercentage");
    set_bright->setIcon(QIcon(":/22x22/apps/setting.svg"));
    set_bright->setText("SetBrightness");

    connect(set_preference,&QAction::triggered,this,&MainWindow::set_preference_func);
    connect(show_percentage,&QAction::triggered,this,&MainWindow::show_percentage_func);
    menu->addAction(show_percentage);
    menu->addAction(set_bright);
    menu->addAction(set_preference);

    trayIcon->setContextMenu(menu);
    ed->power_device_recalculate_icon();
    trayIcon->show();

}

void MainWindow::onSumChanged(QString str)
{
    trayIcon->setToolTip(str);
    qDebug()<<str;
}

void MainWindow::discharge_notify(DEV dev)
{
    Q_UNUSED(dev);
    qDebug()<<"discharge_notify---------";
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    args<<(QCoreApplication::applicationName())
    <<((unsigned int) 0)
    <<QString("qweq")
    <<tr("discharge notify notification")
    <<tr("battery is discharging!")
    <<QStringList()
    <<QVariantMap()
    <<(int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}

void MainWindow::full_charge_notify(DEV dev)
{
    Q_UNUSED(dev);
    qDebug()<<"full_charge_notify---------";
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    args<<(QCoreApplication::applicationName())
    <<((unsigned int) 0)
    <<QString("qweq")
    <<tr("fullly charged notification")
    <<tr("battery is fullly charged!")
    <<QStringList()
    <<QVariantMap()
    <<(int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}

void MainWindow::low_battery_notify(DEV dev)
{
    Q_UNUSED(dev);
    qDebug()<<"low battery notify---------";
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    args<<(QCoreApplication::applicationName())
    <<((unsigned int) 0)
    <<QString("qweq")
    <<tr("low battery notification")
    <<tr("battery is low,please plug in!")
    <<QStringList()
    <<QVariantMap()
    <<(int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}

void MainWindow::critical_battery_notify(DEV dev)
{
    Q_UNUSED(dev);
    qDebug()<<"critical battery notify---------";
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    args<<(QCoreApplication::applicationName())
    <<((unsigned int) 0)
    <<QString("qweq")
    <<tr("critical battery notification")
    <<tr("battery is critical low,please plug in!")
    <<QStringList()
    <<QVariantMap()
    <<(int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}

void MainWindow::action_battery_notify(DEV dev)
{
    Q_UNUSED(dev);

    qDebug()<<"critical battery notify---------";
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    args<<(QCoreApplication::applicationName())
    <<((unsigned int) 0)
    <<QString("qweq")
    <<tr("very low battery notification")
    <<tr("battery is very low,please plug in!")
    <<QStringList()
    <<QVariantMap()
    <<(int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}

void MainWindow::onIconChanged(QString str)
{
        qDebug()<<str<<"trayicon is set";

//    str = ":/22x22/status/"+str+".png";
//    trayIcon->setIcon(QIcon(str));
    QIcon icon = QIcon::fromTheme(str);
    trayIcon->setIcon(icon);
}

void MainWindow::set_preference_func()
{
    QProcess *cmd = new QProcess(this);
    cmd->start("yelp");
}

void MainWindow::show_percentage_func()
{
    QProcess *cmd = new QProcess(this);
    cmd->start("yelp");
}

void MainWindow::onActivatedIcon(QSystemTrayIcon::ActivationReason reason)
{
    //fix here later!
//    get_power_list();
//    QRect rect;
    switch (reason) {
    case QSystemTrayIcon::Trigger:{

            QRect availableGeometry = qApp->primaryScreen()->availableGeometry();
            QRect screenGeometry = qApp->primaryScreen()->geometry();

            QDesktopWidget* pDesktopWidget = QApplication::desktop();
        //    int nScreenCount = QApplication::desktop()->screenCount();
        //    QRect deskRect = pDesktopWidget->availableGeometry();//可用区域
            QRect screenRect = pDesktopWidget->screenGeometry();//屏幕区域

        //    qDebug()<<"screenRect.x(): "<<screenRect.x()<<"   screenRect.height(): "<<screenRect.height();
        //    qDebug()<<"availableGeometry.y(): "<<availableGeometry.y()<<"   availableGeometry.height(): "<<availableGeometry.height();
            if (screenRect.height() != availableGeometry.height()) {
                this->move(availableGeometry.x() + availableGeometry.width() - this->width()-102, availableGeometry.height() - this->height());
            }else {
                this->move(availableGeometry.x() + availableGeometry.width() - this->width()-102, availableGeometry.height() - this->height() - 46);
            }
        if (!this->isHidden()) {
            this->hide();
        }
        else
            this->show();


        break;
    }
    //鼠标左键双击图标
    case QSystemTrayIcon::DoubleClick: {
        this->hide();
        break;
    }
    case QSystemTrayIcon::Context: {
//        if (!this->isHidden()) {
//            this->hide();
//        }
        menu->show();
        break;
    }
    default:
        break;
    }
}



void MainWindow::initData()
{
    saving = false;
    healthing = false;
    pressQss = "QLabel{background-color:#3593b5;}";
    releaseQss = "QLabel{background-color:#283138;}";
}

void MainWindow::initUi2()
{
    setWindowFlags(Qt::FramelessWindowHint|Qt::Popup);
//    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::SplashScreen);
//    setAttribute(Qt::WA_StyledBackground,true);
//    setWindowFlags(Qt::FramelessWindowHint|Qt::Popup);
    resize(360,320);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setWindowOpacity(0.95);

    QLabel *title = new QLabel(tr("PowerManagement"));
//    ui->powertitle->setText(tr("PowerManagement"));
//    QScrollArea *scroll_area = new QScrollArea;
//    scroll_area->setWidget(ui->centralWidget);
    QSpacerItem *spacer = new QSpacerItem(10,10,QSizePolicy::Fixed,QSizePolicy::Expanding);
//     ui->verticalLayout_2->addWidget(title);
//        ui->verticalLayout_2->addSpacerItem(spacer);
    QListWidgetItem *title_item = new QListWidgetItem(ui->listWidget);
    ui->listWidget->setSpacing(3);
//    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setItemWidget(title_item,title);
    get_power_list();

}

int MainWindow::get_engine_dev_number()
{

    int len = ed->devices.size();
    int number = 0;
    for(int i = 0; i < len; i++)
    {
        DEVICE *dv;
        dv = ed->devices.at(i);
        if(dv->m_dev.kind == UP_DEVICE_KIND_LINE_POWER)
            continue;

        number++;
    }
    return  number;
}
void MainWindow::get_power_list()
{
    int j = 0;
    int size;
    size = ed->devices.size();
    for(int i = 0; i < size; i++)
    {
        DEVICE *dv;
        dv = ed->devices.at(i);
        if(dv->m_dev.kind == UP_DEVICE_KIND_LINE_POWER)
            continue;
        QString icon_name = ed->engine_get_device_icon(dv);
        qDebug()<<"this is kind icon-----"<<icon_name;
        QString percentage = QString::number(dv->m_dev.Percentage, 'f',0)+"%";
        bool is_charging = false;
        QString text;
        if(icon_name.contains("charging"))
            is_charging = true;
        /* generate the label */
        if (is_charging)
        {
            text = QString("%1% available power").arg(percentage);
            text.append("\n");
            text.append("(The power is connected and is charging)");
        }
        else
            text = QString("%1% available power").arg(percentage);


        DeviceForm *df = new DeviceForm(this);
        df->set_device(dv);
        QListWidgetItem *list_item = new QListWidgetItem(ui->listWidget);
        list_item->setSizeHint(QSize(325,52));
        ui->listWidget->setItemWidget(list_item,df);
    }

//    for(int i = 0; i < 7; i++)
//    {
//        QString icon_name = "gpm-battery-080-charging.png";
//        QString percentage = QString::number(92.0, 'f',0)+"%";
//        QString state_text = "charging";
//        QString predict = "1 hour 5 minutes";

//        DeviceForm *df= new DeviceForm(this);
//        df->setIcon(icon_name);
//        df->setPercent(percentage);
//        df->slider_changed(int(40));
//        df->setState(state_text);
//        df->setRemain(predict);
//        QListWidgetItem *list_item = new QListWidgetItem(ui->listWidget);
//        list_item->setSizeHint(QSize(325,52));
//        ui->listWidget->setItemWidget(list_item,df);
//    }

}
void MainWindow::iconThemeChanged()
{
    qDebug()<<"icon theme changed";
//    QVariant var = setting->get(POWER_SCHEMA_KEY);
//    QString value = var.value<QString>();
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange) {
        if (QApplication::activeWindow() != this) {
            this->hide();
        }
    }
    return QWidget::event(event);
}

MainWindow::~MainWindow()
{
}



