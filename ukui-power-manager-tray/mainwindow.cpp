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
#include <QDBusReply>

#define POWER_SCHEMA                "org.ukui.power-manager"
#define UKUI_STYLE                  "org.ukui.style"
#define UKUI_STYLE_STYLE_NAME       "style-name"
#define POWER_SCHEMA_KEY            "power-manager"
#define PANEL_DBUS_SERVICE          "com.ukui.panel.desktop"
#define PANEL_DBUS_PATH             "/"
#define PANEL_DBUS_INTERFACE        "com.ukui.panel.desktop"
#define PANEL_SETTINGS              "org.ukui.panel.settings"
#define PANEL_SETTINGS_KEY_HEIGHT   "panelsize"
#define PANEL_SETTINGS_KEY_POSITION "panelposition"

double MainWindow::transparent = 0.7;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ed = EngineDevice::getInstance();
    ui->setupUi(this);
    initData();

    trayIcon = new QSystemTrayIcon(this);
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(onActivatedIcon(QSystemTrayIcon::ActivationReason)));
    connect(ed,SIGNAL(icon_changed(QString)),this,SLOT(onIconChanged(QString)));
    connect(ed,SIGNAL(engine_signal_summary_change(QString)),this,SLOT(onSumChanged(QString)));

    connect(ed,SIGNAL(one_device_add(DEVICE*)),this,SLOT(add_one_device(DEVICE *)));
    connect(ed,SIGNAL(one_device_remove(DEVICE*)),this,SLOT(remove_one_device(DEVICE*)));

    get_window_style();
    initUi();
    ed->engine_policy_settings_cb("iconPolicy");
    ed->engine_recalculate_summary();
//    installEventFilter(this);
}

void MainWindow::onSumChanged(QString str)
{
    Q_UNUSED(str);
    trayIcon->setToolTip(str);
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
    QPixmap result(fmt.width(text), 32);

    QRect rect(0,0,fmt.width(text), 32);
    result.fill(Qt::transparent);
    QPainter painter(&result);
    painter.setFont(m_font);
    painter.setPen(QColor(255,255,255));
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

  QIcon result_icon = QIcon(result_image_h);
  return result_icon;
}

void MainWindow::set_brightness_func()
{
    QProcess::startDetached(QString("ukui-control-center -m &"));

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
//        percent_label->setPixmap(QPixmap(":/apps/tick.png"));
        onIconChanged(ed->previous_icon);
    }
    else {
//        percent_label->setPixmap(QPixmap());
        onIconChanged(ed->previous_icon);
    }

}

void MainWindow::set_window_position( )
{
    QRect rect;
    int availableWidth,totalWidth;
    int availableHeight,totalHeight;
    rect = trayIcon->geometry();
    QRect availableGeometry = qApp->primaryScreen()->availableGeometry();
    QRect screenGeometry = qApp->primaryScreen()->geometry();

    availableWidth = availableGeometry.width();
    availableHeight = availableGeometry.height();
    totalHeight = screenGeometry.height();
    totalWidth = screenGeometry.width();

    int distance = 8;
    int number = QGuiApplication::screens().size();
    if (number > 1)
    {
        QRect main_rect = QGuiApplication::screens().at(0)->geometry();//获取设备屏幕大小
        QRect copy_rect = QGuiApplication::screens().at(1)->geometry();//获取设备屏幕大小
        int n = 0;
        int m = 46;

        n = getTaskbarPos("position");
        m = getTaskbarHeight("height");
        if(n == 0){
            //任务栏在下侧
            availableWidth = screenGeometry.x()+screenGeometry.width();
            availableHeight = screenGeometry.y()+screenGeometry.height();
            setGeometry(availableWidth-this->width()-distance,availableHeight-this->height()-m-distance,this->width(),this->height());

        }else if(n == 1){
            //任务栏在上侧
            availableWidth = screenGeometry.x()+screenGeometry.width();
            availableHeight = screenGeometry.y()+screenGeometry.height();
            setGeometry(availableWidth-this->width()-distance,screenGeometry.y()+m+distance,this->width(),this->height());

        } else if (n == 2){
            //任务栏在左侧
            availableWidth = screenGeometry.x()+screenGeometry.width();
            availableHeight = screenGeometry.y()+screenGeometry.height();
            setGeometry(screenGeometry.x()+m+distance,availableHeight-this->height()-distance,this->width(),this->height());
        } else if (n == 3){
            //任务栏在右侧
            availableWidth = screenGeometry.x()+screenGeometry.width();
            availableHeight = screenGeometry.y()+screenGeometry.height();
            setGeometry(availableWidth-this->width()-m-distance,availableHeight-this->height()-distance,this->width(),this->height());
        }
    }
    else if(totalWidth == availableWidth )//down and up
    {
        if (rect.y() > availableHeight/2){
            if (availableWidth - rect.x() - distance < this->width())
                this->setGeometry(availableWidth-this->width()-distance,availableHeight-this->height()-distance,this->width(),this->height());
            else
                this->setGeometry(rect.x(),availableHeight-this->height()-distance,this->width(),this->height());
        }else{
            if (availableWidth - rect.x() - distance < this->width())
                this->setGeometry(availableWidth-this->width()-distance,totalHeight-availableHeight+distance,this->width(),this->height());
            else
                this->setGeometry(rect.x(),totalHeight-availableHeight+distance,this->width(),this->height());
        }
    }
    else if (totalHeight == availableHeight)//right and left
    {
        if (rect.x() > availableWidth/2){
            if (availableHeight - rect.y() -distance > this->height() )
                this->setGeometry(availableWidth - this->width() - distance,rect.y(),this->width(),this->height());
            else
                this->setGeometry(availableWidth - this->width() - distance,totalHeight - this->height() -distance,this->width(),this->height());
        } else {
            if (availableHeight - rect.y() -distance > this->height() )
                this->setGeometry(totalWidth - availableWidth + distance,rect.y(),this->width(),this->height());
            else
                this->setGeometry(totalWidth-availableWidth+distance,totalHeight - this->height() -distance,this->width(),this->height());
        }
    }
}


void MainWindow::onActivatedIcon(QSystemTrayIcon::ActivationReason reason)
{
    QString style_string;
    switch(reason) {
        case QSystemTrayIcon::Trigger: {
                if (!this->isHidden()) {
                    this->hide();
                }
                else
                {
                    transparent = get_window_opacity();
//                    if(style_nm == "ukui-dark")
//                    {
//                        style_string = QString("#centralWidget {"
//                                                       "background-color:rgba(19,19,20,%1);"
//                                                       "border-radius:6px;}").arg(transparent);
//                    }
//                    else
//                    {
//                        style_string = QString("#centralWidget {"
//                                                       "background-color:rgba(255,255,255,%1);"
//                                                       "border-radius:6px;}").arg(transparent);

//                    }
//                    setStyleSheet(style_string);

                    this->showNormal();
                    set_window_position();
                }
                break;
        }

        default:
            this->hide();
            break;
    }
}

void MainWindow::initData()
{
    want_percent = false;
}

void MainWindow::initUi()
{
    setWindowFlags(Qt::FramelessWindowHint|Qt::Popup);
    setAttribute(Qt::WA_StyledBackground,true);
    setAttribute(Qt::WA_TranslucentBackground);

    QPainterPath path;
    QRect rect = this->rect();
    rect.adjust(1,1,-1,-1);
    path.addRoundedRect(rect,6,6);
    setProperty("blurRegion",QRegion(path.toFillPolygon().toPolygon()));

    dev_number = get_engine_dev_number();
    resize(360,76 + 8 + 82*(dev_number>3?3:dev_number));

    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setSpacing(0);
    ui->listWidget->setContentsMargins(0,0,4,0);
    ui->mainVerticalLayout->setContentsMargins(18,22,14,2);
    ui->mainVerticalLayout->setSpacing(0);
    ui->horizontalLayout->setContentsMargins(0,0,4,0);
    ui->horizontalLayout->setSpacing(0);
    ui->widget->setFixedHeight(30);
    if(dev_number > 3)
    {
        ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }
    else
    {
        ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    ui->power_title->setText(tr("PowerManagement"));
    ui->power_title->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

    connect(ui->statistic_button,SIGNAL(released()),this,SLOT(activate_power_statistic()));

    ui->statistic_button->setText(tr("Stats"));//adapt to chinese
    ui->statistic_button->setFixedWidth(54);
    ui->statistic_button->setStyleSheet("QPushButton{border:0px;width:54px;"
                                        "font-family:Noto Sans CJK SC;"
                                        "background-color:rgba(255,255,255,0);color:rgba(107,142,235,1);font-size:14px;}"
                                        "QPushButton::hover {background-color:rgba(255,255,255,0);color:rgba(151,175,241,1);font-size:14px;}"
                                        "QPushButton::pressed {background-color:rgba(255,255,255,0);color:rgba(61,107,229,1);font-size:12px;}"
                                        );
    transparent = get_window_opacity();
//    QString style_string = QString("#centralWidget {"
//                                   "background-color:rgba(19,19,20,%1);"
//                                   "border-radius:6px;}").arg(transparent);
//    setStyleSheet(style_string);

    get_power_list();

    menu = new QMenu(this);
    menu->setAttribute(Qt::WA_TranslucentBackground);
    menu->setWindowFlag(Qt::FramelessWindowHint);

    create_menu_item();
    trayIcon->setContextMenu(menu);
//    trayIcon->setToolTip(tr("PowerManager"));
}

void MainWindow::create_menu_item()
{
    QAction *pset_preference  = new QAction(menu);
    QAction *pset_bright = new QAction(menu);

    QIcon icon = QIcon::fromTheme("document-page-setup");
    pset_preference->setIcon(icon);
    pset_preference->setText(tr("SetPower"));

    pset_bright->setIcon(icon);
    pset_bright->setText(tr("SetBrightness"));

    connect(pset_preference,&QAction::triggered,this,&MainWindow::set_preference_func);
    connect(pset_bright,&QAction::triggered,this,&MainWindow::set_brightness_func);
    menu->addAction(pset_bright);
    menu->addAction(pset_preference);
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

        DeviceForm *df = new DeviceForm(this);
        df->set_device(dv);
        QListWidgetItem *list_item = new QListWidgetItem(ui->listWidget);
        list_item->setFlags(Qt::NoItemFlags);
        list_item->setSizeHint(QSize(324,82));
        ui->listWidget->setItemWidget(list_item,df);
        device_item_map.insert(dv,list_item);
    }

}

void MainWindow::add_one_device(DEVICE *device)
{
    DeviceForm *df = new DeviceForm(this);
    df->set_device(device);
    QListWidgetItem *list_item = new QListWidgetItem(ui->listWidget);
    list_item->setSizeHint(QSize(324,82));
    list_item->setFlags(Qt::NoItemFlags);
    ui->listWidget->setItemWidget(list_item,df);
    device_item_map.insert(device,list_item);
    dev_number ++;
    if(dev_number > 3)
    {
        ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }
    else
    {
        resize(360,76 + 8 + 82*dev_number);
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
        device->deleteLater();
        device = NULL;
        delete del_item;
        del_item = NULL;
        dev_number --;
        if(dev_number > 3)
        {
            ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        }
        else
        {
            ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            if(dev_number < 3)
            {
                resize(360,76 + 8 + 82*dev_number);
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

//bool MainWindow::eventFilter(QObject *watched, QEvent *event)
//{
//    if(QEvent::WindowDeactivate == event->type())
//    {
//        hide();
//        return true;
//    }
//    else
//        return QWidget::eventFilter(watched,event);
//}

void MainWindow::paintEvent(QPaintEvent *event)
{
//    transparent = get_window_opacity();
    QPainter p(this);
    QStyleOption opt;
    opt.init(this);
    p.setBrush(opt.palette.color(QPalette::Base));
    p.setPen(Qt::NoPen);
    p.setOpacity(transparent);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawRoundedRect(rect(),6,6);
    QWidget::paintEvent(event);
}

MainWindow::~MainWindow()
{
}

int MainWindow::getTaskbarPos(QString str)
{
    QDBusInterface interface( "com.ukui.panel.desktop",
                              "/",
                              "com.ukui.panel.desktop",
                              QDBusConnection::sessionBus() );

    QDBusReply<int> reply = interface.call("GetPanelPosition", str);
    return reply;
}

int MainWindow::getTaskbarHeight(QString str)
{
    QDBusInterface interface( "com.ukui.panel.desktop",
                              "/",
                              "com.ukui.panel.desktop",
                              QDBusConnection::sessionBus() );

    QDBusReply<int> reply = interface.call("GetPanelSize", str);
    return reply;
}

double MainWindow::get_window_opacity()
{
    double t = 0.7;
    if (QGSettings::isSchemaInstalled("org.ukui.control-center.personalise"))
    {

        QGSettings settings("org.ukui.control-center.personalise");
        QStringList keys = settings.keys();
        if(keys.contains("transparency")){
           t = settings.get("transparency").toDouble();
        }
    }
    return t;
}

QString MainWindow::get_window_style()
{
    style_nm = "ukui-dark";
    if (QGSettings::isSchemaInstalled(GPM_SETTINGS_SCHEMA_STYLE))
    {
        style_set = new QGSettings(GPM_SETTINGS_SCHEMA_STYLE);
        connect(style_set,SIGNAL(changed(const QString&)),this,SLOT(style_name_settings_cb(const QString&)));

        QStringList keys = style_set->keys();
        if(keys.contains(GPM_SETTINGS_STYLE_NAME)){
           style_nm = style_set->get(GPM_SETTINGS_STYLE_NAME).toString();
        }
    }
    return style_nm;
}

void MainWindow::style_name_settings_cb(const QString&)
{
    QVariant var =style_set->get(GPM_SETTINGS_STYLE_NAME);
    style_nm = var.value<QString>();
    Q_EMIT style_modify(style_nm);
//    if(style_nm == "ukui-dark")
//        ;
//    else if(style_nm == "ukui-white")
//        ;
}
