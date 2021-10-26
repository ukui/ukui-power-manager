/*
 * Copyright 2021 KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "powerwindow.h"
#include "enginedevice.h"

#include <QApplication>
#include <QTimer>
#include "qdesktopwidget.h"

#define PANEL_DBUS_SERVICE "com.ukui.panel.desktop"
#define PANEL_DBUS_PATH "/"
#define PANEL_DBUS_INTERFACE "com.ukui.panel.desktop"
#define UKUI_PANEL_SETTING "org.ukui.panel.settings"

#define ORG_UKUI_STYLE            "org.ukui.style"
#define STYLE_NAME                "styleName"

#define DBUS_NAME       "org.ukui.SettingsDaemon"
#define DBUS_PATH       "/org/ukui/SettingsDaemon/wayland"
#define MARGIN 4

powerwindow::powerwindow(QWidget *parent) : QWidget(parent)
{
    initgsetting();

    m_IconBatterChangeList   << KYLIN_BATTERY_CHARGING0_PATH1  << KYLIN_BATTERY_CHARGING10_PATH1
                             << KYLIN_BATTERY_CHARGING20_PATH1 << KYLIN_BATTERY_CHARGING30_PATH1
                             << KYLIN_BATTERY_CHARGING40_PATH1 << KYLIN_BATTERY_CHARGING50_PATH1
                             << KYLIN_BATTERY_CHARGING60_PATH1 << KYLIN_BATTERY_CHARGING70_PATH1
                             << KYLIN_BATTERY_CHARGING80_PATH1 << KYLIN_BATTERY_CHARGING90_PATH1
                             << KYLIN_BATTERY_CHARGING100_PATH1 << KYLIN_BATTERY_00_PATH1;
    ed = EngineDevice::getInstance();
    connect(ed,SIGNAL(engine_signal_charge(DEV)),this,SLOT(charge_notify(DEV)));
    connect(ed,SIGNAL(engine_signal_discharge(DEV)),this,SLOT(discharge_notify(DEV)));
    connect(ed,SIGNAL(engine_signal_fullycharge(DEV)),this,SLOT(full_charge_notify(DEV)));
    connect(ed,SIGNAL(engine_signal_charge_low(DEV)),this,SLOT(low_battery_notify(DEV)));
    connect(ed,SIGNAL(engine_signal_charge_critical(DEV)),this,SLOT(critical_battery_notify(DEV)));
    connect(ed,SIGNAL(engine_signal_charge_action(DEV)),this,SLOT(action_battery_notify(DEV)));
//    connect(ed, SIGNAL(icon_changed(QString)), this,SLOT(IconChanged(QString)));
    QDBusConnection::sessionBus().connect(QString(),"/","org.ukui.upower",
                                          "BatteryIcon",this,
                                          SLOT(IconChanged(QString))
                                          );
    setWindowProperty(); //设置窗口属性
    initUI();            //初始化UI
    set_window_position();       //设置出现在屏幕的位置以及大小
//    ed->engine_policy_settings_cb("iconPolicy");
    ed->engine_recalculate_summary();

    connect(QApplication::desktop(), &QDesktopWidget::resized, this,[=](){
        QTimer::singleShot(1000,this,[=](){set_window_position();});
    });
    connect(QApplication::desktop(), &QDesktopWidget::screenCountChanged, this, &powerwindow::set_window_position);
    connect(qApp,&QApplication::primaryScreenChanged,this,&powerwindow::set_window_position);


}

powerwindow::~powerwindow()
{
    delete m_pmainlayout;
    delete m_firstlayout;
    delete lastlayout;
    delete m_firstwidget;
    delete iconLabel;
    delete iconButton;
    delete percentageLabel;
    delete powerStateLabel;
    delete powerTimeToEmpty;
    delete line;
    delete settingLabel;
}

void powerwindow::setWindowProperty()
{
    //设置任务栏无显示
    setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint | Qt::Popup);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    setWindowOpacity(1);
    setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    setProperty("useSystemStyleBlur", true);   //设置毛玻璃效果
    this->setFixedSize(360,300);
}

void powerwindow::initUI()
{
    m_pmainlayout = new QVBoxLayout();
    m_firstlayout = new QHBoxLayout(this);
//    m_statelayout = new QHBoxLayout();
    lastlayout    = new QHBoxLayout();

    m_firstwidget = new QWidget();
//    m_statewidget = new QWidget();
    lastWidget    = new QWidget();

    iconLabel = new QLabel();
    iconButton = new QPushButton();

    percentageLabel = new QLabel();

    powerStateLabel = new QLabel();
    powerStateLabel->setText(tr("Charging"));
    powerStateLabel->setVisible(false);

    powerTimeToEmpty = new QLabel();
    powerTimeToEmpty->setVisible(true);


    line = new m_PartLineWidget();
    line->setFixedSize(340,2);
   
    get_power_list();

    this->setFixedSize(360,192 + deviceNum*65);

    //电源设置按钮
    settingLabel = new settinglabel();
    connect(settingLabel,&settinglabel::labelclick,this,&powerwindow::set_preference_func);

    //第一行布局
    m_firstlayout->addWidget(iconButton,0,Qt::AlignLeft | Qt::AlignCenter);
    m_firstlayout->addStretch();
    m_firstlayout->addWidget(percentageLabel,0,Qt::AlignLeft | Qt::AlignCenter);
    m_firstlayout->addStretch();
    m_firstlayout->addItem(new QSpacerItem(100,5, QSizePolicy::Expanding, QSizePolicy::Minimum));
    m_firstlayout->addWidget(powerTimeToEmpty,0,Qt::AlignLeft | Qt::AlignCenter);
    m_firstlayout->addWidget(powerStateLabel,0,Qt::AlignLeft | Qt::AlignCenter);
    m_firstlayout->addStretch();

    m_firstwidget->setLayout(m_firstlayout);


    //电源设置按钮布局
    lastlayout->addWidget(settingLabel);
    lastlayout->addItem(new QSpacerItem(300,2));
    lastWidget->setLayout(lastlayout);

    //总体布局
    m_pmainlayout->addWidget(m_firstwidget);
    m_pmainlayout->addWidget(line);
    m_pmainlayout->addWidget(lastWidget);
    this->setLayout(m_pmainlayout);
    
}

void powerwindow::initgsetting()
{
    const QByteArray style_id(ORG_UKUI_STYLE);
    QGSettings *style_settings = new QGSettings(style_id);
    if(QGSettings::isSchemaInstalled(style_id)){
        style_settings = new QGSettings(style_id);
    }
}

void powerwindow::get_power_list()
{
    int size;
    size = ed->devices.size();
    for(int i = 0; i < size; i++)
    {
        DEVICE *dv;
        dv = ed->devices.at(i);

        //通过提示信息，新建widget
        if(dv->m_dev.kind == UP_DEVICE_KIND_LINE_POWER){
            continue;
        }

        if(dv->m_dev.kind ==UP_DEVICE_KIND_BATTERY)
            {
            //设置百分比大小
            QFont ft;
            ft.setPointSize(28);
            percentageLabel->setFont(ft);
            percentageLabel->setText(QString("%1%").arg(dv->m_dev.Percentage));

            powerStateLabel->setVisible(true);
            powerTimeToEmpty->setVisible(false);
            }

        connect(ed,&EngineDevice::engine_signal_Battery_State,this,&powerwindow::onBatteryChanged);
            continue;
        }

}

void powerwindow::set_preference_func()
{
    QProcess::startDetached(QString("ukui-control-center -p &"));

}
void powerwindow::batteryChange(int dev)
{
    if(dev == 4 || dev == 1 || dev == 5) {
        powerStateLabel->setVisible(true);
        powerTimeToEmpty->setVisible(false);
    } else {
        powerStateLabel->setVisible(false);
        powerTimeToEmpty->setVisible(true);
    }
}

void powerwindow::onBatteryChanged(QStringList args)
{
//    qDebug()<<"电脑电池电量与状态："<<args;
    settings = new QGSettings(GPM_SETTINGS_SCHEMA);
    int battery = args.at(0).toInt();
    int state = args.at(1).toInt();
    int timeToEmpty = args.at(2).toInt();
    int is_show = settings->get(GPM_SETTINGS_DISPLAY_LEFT_TIME).toInt();
    if (is_show) {
        powerTimeToEmpty->setText(QString(tr("Left %1h \n %2m")).arg((timeToEmpty)/3600).arg(((timeToEmpty)%3600)/60));
    }
    else{
        powerTimeToEmpty->setText(QString(tr("Discharging")));
    }
    if (state == 1 || state == 5){
        powerStateLabel->setText(tr("Charging"));

    }
    else if (state == 4){
        powerStateLabel->setText(tr("fully charged"));
    }
    batteryChange(state);
    percentageLabel->setText(QString("%1%").arg(battery));
}

void powerwindow::IconChanged(QString str){
    if (!str.isNull()) {
        QIcon icon = QIcon::fromTheme(str);
        iconButton->setIcon(icon);
	iconButton->setIconSize(QSize(36,36));
//	iconButton->setProperty("useIconHighlightEffect",0x2);
	//iconButton->setProperty("useBuutonPalette",true);
//	iconButton->setProperty("isWindowButton",0x1);
	iconButton->setFlat(true);
    }

}



void powerwindow::charge_notify(DEV dev)
{
    QString icon = ed->engine_get_dev_icon(dev);
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    args<<tr("Power Manager")
    <<((unsigned int) 0)
    <<"ukui-power-manager"
    <<tr("charge notification")
    <<tr("battery is charging")
    <<QStringList()
    <<QVariantMap()
    <<(int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}

void powerwindow::discharge_notify(DEV dev)
{
    QString icon = ed->engine_get_dev_icon(dev);
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    args<<tr("Power Manager")
    <<((unsigned int) 0)
    <<"ukui-power-manager"
    <<tr("discharged notification")
    <<tr("battery is discharging")
    <<QStringList()
    <<QVariantMap()
    <<(int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}

void powerwindow::full_charge_notify(DEV dev)
{
    QString icon = ed->engine_get_dev_icon(dev);
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    args<<tr("Power Manager")
    <<((unsigned int) 0)
//    <<QString("battery-level-100-symbolic")
    <<"ukui-power-manager"
    <<tr("fullly charged notification")
    <<tr("battery is fullly charged")
    <<QStringList()
    <<QVariantMap()
    <<(int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}

void powerwindow::low_battery_notify(DEV dev)
{
    QString icon = ed->engine_get_dev_icon(dev);
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    args<<tr("Power Manager")
    <<((unsigned int) 0)
    <<"ukui-power-manager"
    <<tr("low battery notification")
    <<tr("battery is low, please plug in power")
    <<QStringList()
    <<QVariantMap()
    <<(int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}

void powerwindow::critical_battery_notify(DEV dev)
{
    Q_UNUSED(dev);
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    args<<tr("Power Manager")
    <<((unsigned int) 0)
    <<ed->power_device_get_icon()
    <<tr("critical battery notification")
    <<tr("battery is critical low,please plug in!")
    <<QStringList()
    <<QVariantMap()
    <<(int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}

void powerwindow::action_battery_notify(DEV dev)
{
    QString icon = ed->engine_get_dev_icon(dev);
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    args<<tr("Power Manager")
    <<((unsigned int) 0)
    <<"ukui-power-manager"
    <<tr("operation notification")
    <<tr("performing low power operation")
    <<QStringList()
    <<QVariantMap()
    <<(int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}


void powerwindow::set_window_position()
{
    QDBusInterface iface("org.ukui.panel",
                         "/panel/position",
                         "org.ukui.panel", QDBusConnection::sessionBus());
    QDBusReply < QVariantList > reply =iface.call("GetPrimaryScreenGeometry");
//    qDebug() << reply.value().at(2).toInt();
    switch (reply.value().at(4).toInt()) {
    case 1:
        this->setGeometry(reply.value().at(0).toInt() +
                          reply.value().at(2).toInt() - this->width() -
                          MARGIN, reply.value().at(1).toInt() + MARGIN,
                          this->width(), this->height());
        break;
    case 2:
        this->setGeometry(reply.value().at(0).toInt() + MARGIN,
                          //position_list.at(1).toInt()+reply.value().at(3).toInt()-this->height()-MARGIN,
                          reply.value().at(3).toInt()- this->height() - MARGIN,
                          this->width(), this->height());
        break;
    case 3:
        this->setGeometry(reply.value().at(2).toInt() - this->width() - MARGIN,
                          reply.value().at(3).toInt()-this->height()- MARGIN,
                          this->width(), this->height());
        break;
    default:
        this->setGeometry(reply.value().at(0).toInt() +
                          reply.value().at(2).toInt() - this->width() -
                          MARGIN,
                          reply.value().at(1).toInt() +
                          reply.value().at(3).toInt() - this->height() -
                          MARGIN, this->width(), this->height());
        break;
    }

}

void powerwindow::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    QRect rect = this->rect();
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(opt.palette.color(QPalette::Base));
    p.setOpacity(0.7);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect,6,6);
    QWidget::paintEvent(e);
}

m_PartLineWidget::m_PartLineWidget(QWidget *parent) : QWidget(parent)
{

}

void m_PartLineWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    QRect rect = this->rect();

    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    QColor color=qApp->palette().color(QPalette::Base);
    if(color.red() == 255 && color.green() == 255 && color.blue() == 255){
        color.setRgb(1,1,1,255);
    } else if (color.red() == 31 && color.green() == 32 && color.blue() == 34) {
        color.setRgb(255,255,255,255);
    }
    p.setBrush(color);
    p.setOpacity(0.05);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect,0,0);
    QWidget::paintEvent(event);
}

//滑动条组件
stateslider::stateslider(QWidget *parent) : QSlider(parent)
{
    setOrientation(Qt::Orientation::Horizontal);
    setMaximum(2);/*
    setMinimumHeight(5);
    setMinimumWidth(340);*/

    setStyleSheet(
           "QSlider::groove:horizontal {"
            "border: 0px none;"
            "background: #262626;"
            "height: 5px;"
            "width: 340px;"
            "border-radius: 5px;"
            "}"

            "QSlider::handle:horizontal {"
                "border: 1px solid #5c5c5c;"
                "width: 10px;"
                "margin: -10px 0;"
                "border-radius: 1px; "
                "background-color: #05B8CC;"
            "}"

            "QSlider::add-page:horizontal {"
            "background: #262626;"
            "border-radius: 5px;"
            "}"


        );
}

//设置label
settinglabel::settinglabel() : QLabel()
{

    QPalette pe;
    pe.setColor(QPalette::WindowText,QColor(68,131,219));
    this->setPalette(pe);
    this->setText(tr("PowerSet"));

}

//鼠标点击事件
void settinglabel::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton){
        Q_EMIT labelclick();
    }
    return;
}


