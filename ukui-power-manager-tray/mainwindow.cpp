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
#include <QPainter>
#include <QFile>
#include <customstyle.h>

#define POWER_SCHEMA "org.ukui.power-manager"
#define POWER_SCHEMA_KEY "power-manager"
#define PANEL_DBUS_SERVICE "com.ukui.panel.desktop"
#define PANEL_DBUS_PATH "/"
#define PANEL_DBUS_INTERFACE "com.ukui.panel.desktop"

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
//    connect(ed,SIGNAL(engine_signal_charge_low(DEV)),this,SLOT(low_battery_notify(DEV)));
//    connect(ed,SIGNAL(engine_signal_charge_critical(DEV)),this,SLOT(critical_battery_notify(DEV)));
//    connect(ed,SIGNAL(engine_signal_charge_action(DEV)),this,SLOT(action_battery_notify(DEV)));
//    connect(ed,SIGNAL(engine_signal_discharge(DEV)),this,SLOT(discharge_notify(DEV)));
//    connect(ed,SIGNAL(engine_signal_fullycharge(DEV)),this,SLOT(full_charge_notify(DEV)));

    connect(ed,SIGNAL(one_device_add(DEVICE*)),this,SLOT(add_one_device(DEVICE *)));
    connect(ed,SIGNAL(one_device_remove(DEVICE*)),this,SLOT(remove_one_device(DEVICE*)));

    initUi();
    ed->engine_policy_settings_cb("iconPolicy");
    panel_height = 0;
    serviceInterface = new QDBusInterface(PANEL_DBUS_SERVICE,
                                        PANEL_DBUS_PATH,
                                        PANEL_DBUS_INTERFACE,
                                        QDBusConnection::sessionBus());

}

void MainWindow::onSumChanged(QString str)
{
    Q_UNUSED(str);
//    trayIcon->setToolTip(str);
}

void MainWindow::discharge_notify(DEV dev)
{
    Q_UNUSED(dev);
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
    /*if(!str.isNull())
    {
        QIcon icon = QIcon::fromTheme(str);
        trayIcon->setIcon(icon);
        trayIcon->show();

        str = ":/status/"+str+".png";
        QPixmap a(str);
        a = a.scaled(32,80);
        QIcon icon(a);
        trayIcon->setIcon(icon);
        trayIcon->show();
    }
    else {
        trayIcon->hide();
    }*/

    want_percent = false;
    if(!str.isNull())
    {
        QIcon icon = QIcon::fromTheme(str);
        if(!want_percent)
        {
            trayIcon->setIcon(icon);
            trayIcon->show();
        }
        else {
            if(ed->composite_device == NULL)
                return;
            QIcon merge_icon = get_percent_icon(icon);
            trayIcon->setIcon(merge_icon);
            trayIcon->show();
        }
    }
    else {
        trayIcon->hide();
    }
}

QPixmap MainWindow::set_percent_pixmap(QString text)
{
    QFont m_font("Arial");
    QFontMetrics fmt(m_font);
//    QPixmap result(fmt.width(text), fmt.height());
    QPixmap result(fmt.width(text), 32);

    QRect rect(0,0,fmt.width(text), 32);
    result.fill(Qt::transparent);
    QPainter painter(&result);
    painter.setFont(m_font);
    painter.setPen(QColor(255,255,255));
    //painter.drawText(const QRectF(fmt.width(text), fmt.height()),Qt::AlignLeft, text);
    painter.drawText(rect,Qt::AlignVCenter|Qt::AlignLeft,text);

    return result;

}

QIcon MainWindow::get_percent_icon(QIcon icon)
{
  int max_width=0;
  int max_height = 0;
  if(ed->composite_device == NULL)
      return icon;
  QString text = QString::number(ed->composite_device->m_dev.Percentage,'f',0) + "%";
  QPixmap perct = set_percent_pixmap(text);
  QPixmap icon_map = icon.pixmap(32,32);
  max_height = icon_map.height();
  max_width = perct.width()+2 + icon_map.width();


  QPixmap result_image_h(max_width,max_height);
  result_image_h.fill(Qt::transparent);

  QPainter painter_h;
  painter_h.begin(&result_image_h);
  int x_number=0;

  painter_h.drawPixmap(x_number,0,icon_map);
  x_number += icon_map.width();
  x_number +=2;
  painter_h.drawPixmap(x_number,0,perct);

  painter_h.end();

//  QFile file("percent.png");
//  file.open(QIODevice::WriteOnly);
//  result_image_h.save(&file,"PNG");
//  file.close();
  QIcon result_icon = QIcon(result_image_h);
  return result_icon;
}

void MainWindow::set_brightness_func()
{
    QProcess::startDetached(QString("ukui-control-center -d &"));

}

void MainWindow::set_preference_func()
{

    QProcess::startDetached(QString("ukui-control-center -p &"));

}

void MainWindow::show_percentage_func()
{
    want_percent = !want_percent;
    if(want_percent)
    {
        percent_label->setPixmap(QPixmap(":/apps/tick.png"));
        onIconChanged(ed->previous_icon);
    }
    else {
        percent_label->setPixmap(QPixmap());
        onIconChanged(ed->previous_icon);
    }

}

//void MainWindow::onActivatedIcon(QSystemTrayIcon::ActivationReason reason)
//{
//    if(serviceInterface->isValid())
//    {
//        QDBusMessage msg = serviceInterface->call("GetPanelSize", ("height"));
//        if(msg.type() == QDBusMessage::ReplyMessage)
//        {
//            panel_height = msg.arguments().at(0).toInt();
//        }
//        qDebug() << "panel_height" << panel_height;
//    }
//    if(panel_height == 0)
//        panel_height = 46;
//    switch (reason) {
//    case QSystemTrayIcon::Trigger:{

//            QRect availableGeometry = qApp->primaryScreen()->availableGeometry();
//            QRect screenGeometry = qApp->primaryScreen()->geometry();

//            QDesktopWidget* pDesktopWidget = QApplication::desktop();
//        //    int nScreenCount = QApplication::desktop()->screenCount();
//        //    QRect deskRect = pDesktopWidget->availableGeometry();//可用区域
//            QRect screenRect = pDesktopWidget->screenGeometry();//屏幕区域

//            if (screenRect.height() != availableGeometry.height()) {
//                this->move(availableGeometry.x() + availableGeometry.width() - this->width()-102, availableGeometry.height() - this->height() - 3);
//            }else {
//                this->move(availableGeometry.x() + availableGeometry.width() - this->width()-102, availableGeometry.height() - this->height() - panel_height - 3);
//            }
//        if (!this->isHidden()) {
//            this->hide();
//        }
//        else
//            this->show();


//        break;
//    }
//    //鼠标左键双击图标
//    case QSystemTrayIcon::DoubleClick: {
//        this->hide();
//        break;
//    }
//    case QSystemTrayIcon::Context: {

//        menu->move( menu->pos().x(),menu->pos().y() - 3 );
//        menu->show();
//        break;
//    }
//    default:
//        break;
//    }
//}

void MainWindow::onActivatedIcon(QSystemTrayIcon::ActivationReason reason)
{
    QRect rect;
    int availableWidth,totalWidth;
    int localY,availableHeight,totalHeight;
    rect = trayIcon->geometry();
    qDebug()<<"rectxy:" << rect.x() << rect.y();
    //屏幕可用宽高
    availableWidth = QGuiApplication::screens().at(0)->availableGeometry().width();
    availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
    //总共宽高
    totalWidth =  QGuiApplication::screens().at(0)->size().width();
    totalHeight = QGuiApplication::screens().at(0)->size().height();

    switch(reason) {

        case QSystemTrayIcon::Trigger: {
                if (rect.x() > availableWidth/2 && rect.x()< availableWidth  && rect.y() > availableHeight/2) { //下
                    if (availableWidth - rect.x() - 3 < this->width())
                        this->setGeometry(availableWidth-this->width()-3,availableHeight-this->height()-3,this->width(),this->height());
                    else
                        this->setGeometry(rect.x(),availableHeight-this->height()-3,this->width(),this->height());
                }
                else if (rect.x() > availableWidth/2 && rect.x()< availableWidth && rect.y() < availableHeight/2 ) { //上
                    if (availableWidth - rect.x() - 3 < this->width())
                        this->setGeometry(availableWidth-this->width()-3,totalHeight-availableHeight+3,this->width(),this->height());
                    else
                        this->setGeometry(rect.x(),totalHeight-availableHeight+3,this->width(),this->height());
                }
                else if (rect.x() < availableWidth/2 && rect.y() > availableHeight/2 && rect.y()< availableHeight) { //左
                    localY = availableHeight - this->height();
                    if (availableHeight - rect.y() -3 > this->height() )
                        this->setGeometry(totalWidth - availableWidth + 3,rect.y(),this->width(),this->height());
                    else
                        this->setGeometry(totalWidth-availableWidth+3,localY-3,this->width(),this->height());
                }
                else
                    this->setGeometry(0,0,this->width(),this->height());

                if (!this->isHidden()) {
                    this->hide();
                }
                else
                    this->show();
                break;
        }

        case QSystemTrayIcon::Context: {

            if (rect.x() > availableWidth/2 && rect.x()< availableWidth  && rect.y() > availableHeight/2) { //下
                if (availableWidth - rect.x() - 3 < menu->width())
                {
                    menu->setGeometry(availableWidth-menu->width()-3,availableHeight-menu->height()-3,menu->width(),menu->height());
                }
                else
                {
                    menu->setGeometry(rect.x(),availableHeight-menu->height()-3,menu->width(),menu->height());
                }
            }
            else if (rect.x() > availableWidth/2 && rect.x()< availableWidth && rect.y() < availableHeight/2 ) { //上
                if (availableWidth - rect.x() - 3 < menu->width())
                {
                    menu->setGeometry(availableWidth-menu->width()-3,totalHeight-availableHeight+3,menu->width(),menu->height());
                }
                else
                {
                    menu->setGeometry(rect.x(),totalHeight-availableHeight+3,menu->width(),menu->height());
                }
            }
            else if (rect.x() < availableWidth/2 && rect.y() > availableHeight/2 && rect.y()< availableHeight) { //左
                localY = availableHeight - menu->height();
                if (availableHeight - rect.y() -3 > menu->height() )
                    menu->setGeometry(totalWidth - availableWidth + 3,rect.y(),menu->width(),menu->height());
                else
                    menu->setGeometry(totalWidth-availableWidth+3,localY-3,menu->width(),menu->height());
            }
            menu->show();
            break;
        }
        default:
            this->hide();
            menu->hide();
            break;
    }

}
void MainWindow::initData()
{
    want_percent = false;
    saving = false;
    healthing = false;
    pressQss = "QLabel{background-color:#3593b5;}";
    releaseQss = "QLabel{background-color:#283138;}";
}

void MainWindow::initUi()
{
    setWindowFlags(Qt::FramelessWindowHint|Qt::Popup);
//    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::SplashScreen);
    setAttribute(Qt::WA_StyledBackground,true);
    setAttribute(Qt::WA_TranslucentBackground);
    QPainterPath path;
    auto rect = this->rect();
    rect.adjust(0,0,0,0);
    path.addRoundedRect(rect,6,6);
    setProperty("blurRegion",QRegion(path.toFillPolygon().toPolygon()));

    setWindowOpacity(0.90);
    dev_number = get_engine_dev_number();
    dev_number = 1;
    resize(360,72 + 82*(dev_number>3?3:dev_number));
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    if(dev_number > 3)
    {
        ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }
    else
    {
        ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    ui->power_title->setText(tr("PowerManagement"));
    ui->power_title->setAlignment(Qt::AlignLeft);

    connect(ui->statistic_button,SIGNAL(clicked()),this,SLOT(activate_power_statistic()));

    ui->statistic_button->setText(tr("PowerStatistics"));//adapt to chinese
    ui->statistic_button->setFixedWidth(54);

    ui->statistic_button->setStyleSheet("QPushButton{border:0px;width:54px;"
                                        "font-family:Noto Sans CJK SC;"
                                        "background-color:rgba(255,255,255,0);color:rgba(107,142,235,1);font-size:14px;}"
                                        "QPushButton::hover {background-color:rgba(255,255,255,0);color:rgba(151,175,241,1);font-size:14px;}"
                                        "QPushButton::pressed {background-color:rgba(255,255,255,0);color:rgba(61,107,229,1);font-size:12px;}"
                                        );
    setStyleSheet("#centralWidget {"
                  "background-color:rgba(19,19,20);"
                  "border-radius:6px;}"
                  );
//    QLabel *title = new QLabel(tr("PowerManagement"));
//    title->setObjectName("power_title");
//    QScrollArea *scroll_area = new QScrollArea;
//    scroll_area->setWidget(ui->centralWidget);
//    QSpacerItem *spacer = new QSpacerItem(10,10,QSizePolicy::Fixed,QSizePolicy::Expanding);
//    ui->verticalLayout_2->addWidget(title);
//    ui->verticalLayout_2->addSpacerItem(spacer);
//    QListWidgetItem *title_item = new QListWidgetItem(ui->listWidget);
//    ui->listWidget->setSpacing(30);
//    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    ui->listWidget->setItemWidget(title_item,title);
//    title_item->setSizeHint(QSize(325,20));
    get_power_list();

    menu = new QMenu(this);
    menu->setWindowOpacity(0.90);
    menu->setAttribute(Qt::WA_TranslucentBackground);
    menu->setWindowFlag(Qt::FramelessWindowHint);
    set_preference  = new QWidgetAction(menu);
    show_percentage = new QWidgetAction(menu);
    set_bright = new QWidgetAction(menu);

    QHBoxLayout *hbox_preference = new QHBoxLayout;
    QLabel *preference_label = new QLabel(this);
    QLabel *preference_text = new QLabel(this);
    QWidget *preference_widget = new QWidget(this);
    preference_label->setFixedSize(QSize(16,16));
    preference_label->setPixmap(QPixmap(":/apps/setting.svg"));
    preference_text->setText(tr("SetPower"));
    preference_label->setStyleSheet("QLabel{background:transparent;border:0px;}");
    preference_text->setStyleSheet("QLabel{background:transparent;border:0px;}");
    hbox_preference->addWidget(preference_label);
    hbox_preference->addWidget(preference_text);
    preference_widget->setLayout(hbox_preference);
    preference_widget->setFocusPolicy(Qt::NoFocus);
    preference_widget->setFixedSize(QSize(246,36));
    hbox_preference->setSpacing(10);
    set_preference->setDefaultWidget(preference_widget);
    preference_widget->setStyleSheet("QWidget{background:transparent;border:0px;border-radius:4px}\
                                     QWidget:hover{background-color:rgba(255,255,255,0.15);}");

    QHBoxLayout *hbox_percent = new QHBoxLayout;
    percent_label = new QLabel(this);
    QLabel *percent_text = new QLabel(this);
    QWidget *percent_widget = new QWidget(this);
    percent_label->setFixedSize(QSize(16,16));
    percent_label->setPixmap(QPixmap(":/apps/tick.png"));
    percent_text->setText(tr("ShowPercentage"));
    percent_label->setStyleSheet("QLabel{background:transparent;border:0px;}");
    percent_text->setStyleSheet("QLabel{background:transparent;border:0px;}");
    hbox_percent->addWidget(percent_label);
    hbox_percent->addWidget(percent_text);
    percent_widget->setLayout(hbox_percent);
    percent_widget->setFocusPolicy(Qt::NoFocus);
    percent_widget->setFixedSize(QSize(246,36));

    hbox_percent->setSpacing(10);
    show_percentage->setDefaultWidget(percent_widget);
    percent_widget->setStyleSheet("QWidget{background:transparent;border:0px;border-radius:4px}\
                                  QWidget:hover{background-color:rgba(255,255,255,0.15);}");

    QHBoxLayout *hbox_bright= new QHBoxLayout;
    QLabel *bright_label = new QLabel(this);
    QLabel *bright_text = new QLabel(this);
    QWidget *bright_widget = new QWidget(this);
    bright_label->setFixedSize(QSize(16,16));
    bright_label->setPixmap(QPixmap(":/apps/setting.svg"));
    bright_text->setText(tr("SetBrightness"));
    bright_label->setStyleSheet("QLabel{background:transparent;border:0px;}");
    bright_text->setStyleSheet("QLabel{background:transparent;border:0px;}");
    hbox_bright->addWidget(bright_label);
    hbox_bright->addWidget(bright_text);
    bright_widget->setLayout(hbox_bright);
    bright_widget->setFocusPolicy(Qt::NoFocus);
    bright_widget->setFixedSize(QSize(246,36));
    hbox_bright->setSpacing(10);
    set_bright->setDefaultWidget(bright_widget);
    bright_widget->setStyleSheet("QWidget{background:transparent;border:0px;border-radius:4px}"
                                 "QWidget:hover{background-color:rgba(255,255,255,0.15);}");

    connect(set_preference,&QAction::triggered,this,&MainWindow::set_preference_func);
    connect(set_bright,&QAction::triggered,this,&MainWindow::set_brightness_func);
    connect(show_percentage,&QAction::triggered,this,&MainWindow::show_percentage_func);
    menu->addAction(show_percentage);
    menu->addSeparator();
    menu->addAction(set_bright);
    menu->addSeparator();
    menu->addAction(set_preference);

    trayIcon->setContextMenu(menu);
    qApp->setStyle(new CustomStyle());
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
    int size;
    size = ed->devices.size();
    for(int i = 0; i < size; i++)
    {
        DEVICE *dv;
        dv = ed->devices.at(i);
        if(dv->m_dev.kind == UP_DEVICE_KIND_LINE_POWER)
            continue;
        QString icon_name = ed->engine_get_device_icon(dv);
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
        list_item->setSizeHint(QSize(325,82));
        ui->listWidget->setItemWidget(list_item,df);
    }

//    for(int i = 0; i < 1; i++)
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
//        list_item->setSizeHint(QSize(325,82));
//        ui->listWidget->setItemWidget(list_item,df);
//    }

}

void MainWindow::add_one_device(DEVICE *device)
{
    DeviceForm *df = new DeviceForm(this);
    df->set_device(device);
    QListWidgetItem *list_item = new QListWidgetItem(ui->listWidget);
    list_item->setSizeHint(QSize(325,82));
    ui->listWidget->setItemWidget(list_item,df);
    device_item_map.insert(device,list_item);
    dev_number ++;
    if(dev_number > 3)
    {
        ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }
    else
    {
        resize(360,72 + 82*dev_number);
        ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        move(pos().x(),pos().y()-82);
    }

}

void MainWindow::remove_one_device(DEVICE *device)
{

    if(device_item_map.contains(device))
    {
        QListWidgetItem *del_item = device_item_map.value(device);
        QWidget *widget = ui->listWidget->itemWidget(del_item);
        DeviceForm *df = qobject_cast<DeviceForm*>(widget);

        ui->listWidget->removeItemWidget(del_item);
        if(df != NULL)
            df->deleteLater();
        //erase map
        device_item_map.remove(device);
        delete del_item;
        dev_number --;
//        resize(360,72 + 82*(dev_number>3?3:dev_number));
        if(dev_number > 3)
        {
            ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        }
        else
        {
            ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            if(dev_number < 3)
            {
                resize(360,72 + 82*dev_number);
                move(pos().x(),pos().y()+82);
            }
        }
    }
}

void MainWindow::activate_power_statistic()
{
    this->hide();
    QProcess::startDetached(QString("ukui-power-statistics &"));
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

//void MainWindow::paintEvent(QPaintEvent *event)
//{
//    QStyleOption opt;
//    opt.init(this);
//    QPainter p(this);
//    style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,this);
//    QWidget::paintEvent(event);
//}

MainWindow::~MainWindow()
{
    delete serviceInterface;
}



