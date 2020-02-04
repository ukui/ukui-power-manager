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

#define POWER_SCHEMA "org.ukui.power-manager"
#define POWER_SCHEMA_KEY "power-manager"
#define DEBUG

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ed = EngineDevice::getInstance();
    ui->setupUi(this);
    initData();
//    setting = new QGSettings(POWER_SCHEMA);
//    connect(setting,SIGNAL(changed()))
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
    //    qDebug()<<str;

    str = ":/22x22/status/"+str+".png";
    trayIcon->setIcon(QIcon(str));
//    QIcon icon = QIcon::fromTheme(str);
//    trayIcon->setIcon(icon);
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

#if 0
void MainWindow::initUi()
{
    setWindowFlags(Qt::FramelessWindowHint|Qt::Popup);
//    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::SplashScreen);
    setAttribute(Qt::WA_StyledBackground,true);

//    setWindowFlags(Qt::FramelessWindowHint|Qt::Popup);
    resize(315,220);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//    actWidget = new PowerInteract(this);
//    setCentralWidget(actWidget);
//    this->setContentsMargins(0,10,10,0);
    setWindowOpacity(0.95);

    scroll_area = new QScrollArea(ui->centralWidget);
    scroll_area->move(1, 16);
    scroll_area->resize(230, 148-16-61);
    scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_area->show();
    pow_widget = new QWidget(scroll_area);


    ui->brightlb->setStyleSheet("QLabel{background-color:#283138;}");
    ui->savelb->setStyleSheet("QLabel{background-color:#283138;}");
    ui->healthlb->setStyleSheet("QLabel{background-color:#283138;}");


    ui->power_title->setText(tr("PowerManagement"));

    scroll_area->setStyleSheet("QScrollArea{border:none;}");
    scroll_area->viewport()->setStyleSheet("background-color:transparent;");

    ui->brightbtn->setStyleSheet("QPushButton{border:none;}");
    ui->brighttext->setStyleSheet("QLabel{font-size:13px;color:#ffffff;}");
//    ui->brighttext->setStyleSheet("QLabel{font-size:12px;font-weight:100;color:#ffffff;}");

    ui->savebtn->setStyleSheet("QPushButton{border:none;}");
//    ui->savetext->setStyleSheet("QLabel{font-size:13px;color:#ffffff;}");
    ui->savetext->setText(tr("PowerSaveMode"));
    ui->saveicon->setStyleSheet("QLabel{background-image:url(:/22x22/apps/save.png);}");
    ui->healthicon->setStyleSheet("QLabel{background-image:url(:/22x22/apps/health.png);}");
    ui->brighticon->setStyleSheet("QLabel{background-image:url(:/22x22/apps/bright.png);}");

    ui->healthbtn->setStyleSheet("QPushButton{border:none;}");
//    ui->healthtext->setStyleSheet("QLabel{background-image:url(:/res/x/setup.png);}");
    ui->healthtext->setText(tr("BatterySave"));
    ui->brighttext->setText(tr("Brightness"));
    get_power_list();
//    ui->brightlb->setParent(ui->brightbtn);
//    ui->healthlb->setParent(ui->healthbtn);
//    ui->savelb->setParent(ui->savebtn);
}
#endif
void MainWindow::initUi2()
{
    setWindowFlags(Qt::FramelessWindowHint|Qt::Popup);
//    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::SplashScreen);
    setAttribute(Qt::WA_StyledBackground,true);
//    setWindowFlags(Qt::FramelessWindowHint|Qt::Popup);
    resize(360,320);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setWindowOpacity(0.95);





    ui->powertitle->setText(tr("PowerManagement"));


    get_power_list();

}

int MainWindow::get_engine_dev_number()
{
//    QString text;
//    qreal percentage;
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
#if 0
    // chushihua
    if(pow_widget != NULL)
        delete pow_widget;
    pow_widget = new QWidget(scroll_area);
//    pow_widget->resize(230, 151-16-5-61-2);
    pow_widget->resize(315, 151-16-5-61-2);
    scroll_area->setWidget(pow_widget);

    int j = 0;
//    int size = 2;
    int size = get_engine_dev_number();
    if(size==0)
        size = 1;

//    int ht = 5 + 16 + 67 + size*61;
    int ht = 22 + 23 + 5 + size*61 +5 + 60 + 6;
    resize(315,ht);

//    scroll_area->resize(230, ht - 61 - 21);
    scroll_area->resize(315, size*61);
    pow_widget->resize(315, size*61);

    scroll_area->move(1, 50);
    pow_widget->move(1, 50);
    ui->brightlb->move(ui->brightlb->pos().x(),ht - 66);
    ui->healthlb->move(ui->healthlb->pos().x(),ht - 66);
    ui->savelb->move(ui->savelb->pos().x(),ht - 66);
    ui->brightbtn->move(ui->brightbtn->pos().x(),ht - 66);
    ui->healthbtn->move(ui->healthbtn->pos().x(),ht - 66);
    ui->savebtn->move(ui->savebtn->pos().x(),ht - 66);
    ui->saveicon->move(ui->saveicon->pos().x(),ht - 41 - 22);
    ui->savetext->move(ui->savetext->pos().x(),ht - 10 - 14);
    ui->healthicon->move(ui->healthtext->pos().x(),ht - 45 - 22);
    ui->healthtext->move(ui->healthtext->pos().x(),ht - 10 - 15);
    ui->brighticon->move(ui->brighticon->pos().x(),ht - 41 - 22);
    ui->brighttext->move(ui->brighttext->pos().x(),ht - 10 - 14);
#endif


#ifndef DEBUG
    size = ed->devices.size();
    for(int i = 0; i < size; i++)
    {
        DEVICE *dv;
        dv = ed->devices.at(i);
        if(dv->m_dev.kind == UP_DEVICE_KIND_LINE_POWER)
            continue;
        QString icon_name = ed->engine_get_device_icon(dv);
        qDebug()<<"sdfdfsd-----"<<icon_name;
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

        QString state_text = ed->engine_get_state_text(dv->m_dev.State);

        QString predict = ed->engine_get_device_predict(dv);

        DeviceWidget *dw= new DeviceWidget(pow_widget);
        connect(dv,&DEVICE::device_property_changed,[=]{
            dw->icon_name = ed->engine_get_device_icon(dv);
            dw->percentage = QString::number(dv->m_dev.Percentage, 'f',0)+"%";
            dw->state_text = ed->engine_get_state_text(dv->m_dev.State);
            dw->predict = ed->engine_get_device_predict(dv);
            dw->widget_property_change();
        });
        dw->setIcon(icon_name);
        dw->setPercent(percentage);
        dw->setState(state_text);
        dw->setRemain(predict);
        dw->move(0, 3 + j * 61);
        dw->show();
        j++;
    }
#else
    int j = 0;
    for(int i = 0; i < 2; i++)
    {
        QString icon_name = "gpm-battery-080-charging.png";
        QString percentage = QString::number(92.0, 'f',0)+"%";
        QString state_text = "charging";
        QString predict = "1 hour 5 minutes";

        DeviceForm *df= new DeviceForm(this);
        df->setIcon(icon_name);
        df->setPercent(percentage);
        df->setState(state_text);
        df->setRemain(predict);
        df->show();
        ui->vLayout->addWidget(df);

//        DeviceWidget *dw= new DeviceWidget(this);
//        dw->setIcon(icon_name);
//        dw->setPercent(percentage);
//        dw->setState(state_text);
//        dw->setRemain(predict);
//        dw->show();
//        ui->vLayout->addWidget(dw);
    }
#endif

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


void MainWindow::on_savebtn_pressed()
{
    saving = !saving;

}

void MainWindow::on_savebtn_released()
{
//    ui->savelb->setStyleSheet(releaseQss);
}

void MainWindow::on_healthbtn_pressed()
{
    healthing = ! healthing;
}

void MainWindow::on_healthbtn_released()
{
//    ui->healthlb->setStyleSheet(releaseQss);

}

void MainWindow::on_brightbtn_pressed()
{
    static bool bright = true;


}

void MainWindow::on_brightbtn_released()
{
//    ui->brightlb->setStyleSheet(releaseQss);
//    system()
}
