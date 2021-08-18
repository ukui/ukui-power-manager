/*
 *This code just like shit, comments is added by me, but all logic is not my idea. if you have a watch on this comments, I suggest you run rapidly. not hesitate. this company is so lese. of course, I have ran...
 * */

#include "powerwindow.h"

#define PANEL_DBUS_SERVICE "com.ukui.panel.desktop"
#define PANEL_DBUS_PATH "/"
#define PANEL_DBUS_INTERFACE "com.ukui.panel.desktop"
#define UKUI_PANEL_SETTING "org.ukui.panel.settings"

#define ORG_UKUI_STYLE            "org.ukui.style"
#define STYLE_NAME                "styleName"

#define DBUS_NAME       "org.ukui.SettingsDaemon"
#define DBUS_PATH       "/org/ukui/SettingsDaemon/wayland"
#define DBUS_INTERFACE  "org.ukui.SettingsDaemon.wayland"
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
    connect(ed,SIGNAL(one_device_add(DEVICE*)),this,SLOT(add_one_device(DEVICE *)));
    connect(ed,SIGNAL(one_device_remove(DEVICE*)),this,SLOT(remove_one_device(DEVICE*)));
    connect(ed,SIGNAL(engine_signal_charge(DEV)),this,SLOT(charge_notify(DEV)));
    connect(ed,SIGNAL(engine_signal_discharge(DEV)),this,SLOT(discharge_notify(DEV)));
    connect(ed,SIGNAL(engine_signal_fullycharge(DEV)),this,SLOT(full_charge_notify(DEV)));
    connect(ed,SIGNAL(engine_signal_charge_low(DEV)),this,SLOT(low_battery_notify(DEV)));
    connect(ed,SIGNAL(engine_signal_charge_critical(DEV)),this,SLOT(critical_battery_notify(DEV)));
    connect(ed,SIGNAL(engine_signal_charge_action(DEV)),this,SLOT(action_battery_notify(DEV)));
    connect(ed, SIGNAL(icon_changed(QString)), this,SLOT(IconChanged(QString)));

    setWindowProperty(); //设置窗口属性
    initUI();            //初始化UI
    setPosition();       //设置出现在屏幕的位置以及大小
    ed->engine_policy_settings_cb("iconPolicy");
    ed->engine_recalculate_summary();

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
    m_statelayout = new QHBoxLayout();
    lastlayout    = new QHBoxLayout();

    m_firstwidget = new QWidget();
    m_statewidget = new QWidget();
    lastWidget    = new QWidget();

    iconLabel = new QLabel();
    iconButton = new QPushButton();

    percentageLabel = new QLabel();

    powerStateLabel = new QLabel();
    powerStateLabel->setText(tr("Charging"));
    powerStateLabel->setVisible(false);

    powerTimeToEmpty = new QLabel();
    powerTimeToEmpty->setVisible(true);

//    stateSlider = new stateslider();
//
//    enduranceIconLabel = new QLabel();
//    QImage *img2 = new QImage();
//    img2->load(":/charging/battery1/icon-ECO-s.svg");
//    enduranceIconLabel->setPixmap(QPixmap::fromImage(*img2));
//
//    enduranceLabel = new QLabel();
//    enduranceLabel->setText("续航");
//
//    performanceIconLabel = new QLabel();
//    QImage *img3 = new QImage();
//    img3->load(":/charging/battery1/icon-performance1.svg");
//    performanceIconLabel->setPixmap(QPixmap::fromImage(*img3));
//
//    performanceLabel = new QLabel();
//    performanceLabel->setText("性能");

    line = new m_PartLineWidget();
    line->setFixedSize(340,2);

    //设备列表
    listWidget = new QListWidget();
    listWidget->setObjectName(QString::fromUtf8("listWidget"));
    listWidget->setLineWidth(0);
    listWidget->setAutoScroll(false);
    listWidget->setFrameShape(QListWidget::NoFrame);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listWidget->setFocusPolicy(Qt::NoFocus);
    listWidget->setItemAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    listWidget->setStyleSheet("background-color:transparent");
    get_power_list();

    if(deviceNum == 0){
        listWidget->setVisible(false);
    } else {
        listWidget->setVisible(true);
    }
    this->setFixedSize(360,192 + deviceNum*65);

    //电源设置按钮
    settingLabel = new settinglabel();
    connect(settingLabel,&settinglabel::labelclick,this,&powerwindow::set_preference_func);

    //第一行布局
    m_firstlayout->addWidget(iconButton,0,Qt::AlignLeft | Qt::AlignCenter);
    m_firstlayout->addWidget(percentageLabel,0,Qt::AlignLeft | Qt::AlignCenter);
    m_firstlayout->addItem(new QSpacerItem(100,5, QSizePolicy::Expanding, QSizePolicy::Minimum));
    m_firstlayout->addWidget(powerTimeToEmpty,0,Qt::AlignLeft | Qt::AlignCenter);
    m_firstlayout->addWidget(powerStateLabel,0,Qt::AlignLeft | Qt::AlignCenter);
//    m_firstlayout->addItem(new QSpacerItem(100,1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    
    //m_firstlayout->setAlignment(Qt::AlignCenter);
    m_firstwidget->setLayout(m_firstlayout);

    //滑动条下方标识布局
//    m_statelayout->addWidget(enduranceIconLabel);
//    m_statelayout->addWidget(enduranceLabel);
//    m_statelayout->addItem(new QSpacerItem(300,2));
//    m_statelayout->addWidget(performanceIconLabel);
//    m_statelayout->addWidget(performanceLabel);
//    m_statewidget->setLayout(m_statelayout);

    //电源设置按钮布局
    lastlayout->addWidget(settingLabel);
    lastlayout->addItem(new QSpacerItem(300,2));
    lastWidget->setLayout(lastlayout);

    //总体布局
    m_pmainlayout->addWidget(m_firstwidget);
    //m_pmainlayout->addWidget(stateSlider);
    //m_pmainlayout->addWidget(m_statewidget);
    m_pmainlayout->addWidget(line);
    m_pmainlayout->addWidget(listWidget);
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

    connect(style_settings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==STYLE_NAME){
            //处理相应改变
            //qDebug()<<style_settings->get(STYLE_NAME).toString();
        }
    });
}
int powerwindow::hours(int value)
{
    int second;
    int hour;
    hour = second/3600;
    return hour;
}
int powerwindow::minutes(int value)
{
    int second;
    int minute;
    minute = (second%3600)/60;
    return minute;
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
        if(dv->m_dev.kind == UP_DEVICE_KIND_LINE_POWER)
            continue;

        if(dv->m_dev.kind ==UP_DEVICE_KIND_BATTERY) {
            //setBatteryIcon(dv->m_dev.Percentage,dv->m_dev.State);
            //设置百分比大小
            QFont ft;
            ft.setPointSize(28);
            percentageLabel->setFont(ft);
            percentageLabel->setText(QString("%1%").arg(dv->m_dev.Percentage));
            powerTimeToEmpty->setText(QString("剩余%1小时%2分钟").arg(hours(dv->m_dev.TimeToEmpty)).arg(minutes(dv->m_dev.TimeToEmpty)));
            //qDebug()<<dv->m_dev.State;
            if(dv->m_dev.State == 1) {
                powerStateLabel->setVisible(true);
                powerTimeToEmpty->setVisible(false);
            }
	    connect(ed,&EngineDevice::engine_signal_Battery_State,this,&powerwindow::onBatteryChanged);
            continue;
        }

        device_widget *deviceWidget = new device_widget(this);
        deviceWidget->initDevice(dv);
        QListWidgetItem *list_item = new QListWidgetItem(listWidget);
        list_item->setSizeHint(QSize(328,56));
        listWidget->setItemWidget(list_item,deviceWidget);
        device_item_map.insert(dv,list_item);
        deviceNum ++;
    }
}

void powerwindow::set_preference_func()
{
    QProcess::startDetached(QString("ukui-control-center -p &"));

}
void powerwindow::batteryChange(int dev)
{
    if(dev == 4 || dev == 1) {
        powerStateLabel->setVisible(true);
        powerTimeToEmpty->setVisible(false);
    } else {
        powerStateLabel->setVisible(false);
        powerTimeToEmpty->setVisible(true);
    }
}

void powerwindow::onBatteryChanged(QStringList args)
{
    //qDebug()<<"电脑电池电量与状态："<<args;
    int battery = args.at(0).toInt();
    int state = args.at(1).toInt();
    int timeToEmpty = args.at(2).toInt();
    powerTimeToEmpty->setText(QString(tr("Left %1h %2m")).arg(hours(timeToEmpty)).arg(minutes(timeToEmpty)));
    //setBatteryIcon(battery,state);
    batteryChange(state);
    percentageLabel->setText(QString("%1%").arg(battery));
}

void powerwindow::setBatteryIcon(int Ele_surplus_int,int state)
{
    QPixmap pixmap;

//    if(state == 4) {
        if (Ele_surplus_int>=0 && Ele_surplus_int<10) {
            pixmap = m_IconBatterChangeList.at(0);
        } else if (Ele_surplus_int>=10 && Ele_surplus_int<20){
            pixmap = m_IconBatterChangeList.at(1);
        } else if (Ele_surplus_int>=20 && Ele_surplus_int<30){
            pixmap = m_IconBatterChangeList.at(2);
        }else if (Ele_surplus_int>=30 && Ele_surplus_int<40){
            pixmap = m_IconBatterChangeList.at(3);
        }else if (Ele_surplus_int>=40 && Ele_surplus_int<50){
            pixmap = m_IconBatterChangeList.at(4);
        }else if (Ele_surplus_int>=50 && Ele_surplus_int<60){
            pixmap = m_IconBatterChangeList.at(5);
        }else if (Ele_surplus_int>=60 && Ele_surplus_int<70){
            pixmap = m_IconBatterChangeList.at(6);
        }else if (Ele_surplus_int>=70 && Ele_surplus_int<80){
            pixmap = m_IconBatterChangeList.at(7);
        }else if (Ele_surplus_int>=80 && Ele_surplus_int<90){
            pixmap = m_IconBatterChangeList.at(8);
        }else if (Ele_surplus_int>=90 && Ele_surplus_int<100){
            pixmap = m_IconBatterChangeList.at(9);
        }else if (Ele_surplus_int==100){
            pixmap = m_IconBatterChangeList.at(10);
        }
//    } else {
//           pixmap = m_IconBatterChangeList.at(11);
//    }

    iconLabel->setPixmap(pixmap);

}
void powerwindow::IconChanged(QString str){
    if (!str.isNull()) {
        QIcon icon = QIcon::fromTheme(str);
        iconButton->setIcon(icon);
	iconButton->setIconSize(QSize(36,36));
	iconButton->setProperty("useIconHighlightEffect",0x2);
	//iconButton->setProperty("useBuutonPalette",true);
	iconButton->setProperty("isWindowButton",0x1);
	iconButton->setFlat(true);
    }

}

//添加设备
void powerwindow::add_one_device(DEVICE *device)
{
    //qDebug()<<"adddd";
    device_widget *df = new device_widget(this);
    df->initDevice(device);
    QListWidgetItem *list_item = new QListWidgetItem(listWidget);
    list_item->setSizeHint(QSize(328,56));
    listWidget->setItemWidget(list_item,df);
    device_item_map.insert(device,list_item);
    deviceNum ++;

    if(deviceNum == 0){
        listWidget->setVisible(false);
    } else {
        listWidget->setVisible(true);
    }
    this->setFixedSize(360,192 + deviceNum*65);
    GetsAvailableAreaScreen();
}

//移除设备
void powerwindow::remove_one_device(DEVICE *device)
{
    if(device_item_map.contains(device))
    {
        QListWidgetItem *del_item = device_item_map.value(device);
        QWidget *widget = listWidget->itemWidget(del_item);
        device_widget *df = qobject_cast<device_widget*>(widget);

        listWidget->removeItemWidget(del_item);
        if(df != NULL)
            df->deleteLater();
        //erase map
        device_item_map.remove(device);
        device->deleteLater();
        device = NULL;
        delete del_item;
        del_item = NULL;
        deviceNum --;
    }
    if(deviceNum == 0){
        listWidget->setVisible(false);
    } else {
        listWidget->setVisible(true);
    }
    this->setFixedSize(360,192 + deviceNum*65);
    GetsAvailableAreaScreen();
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
    <<(int)15;
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
    <<(int)15;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}


void powerwindow::setPosition()
{
    listenPanelChange();
    QString xdg_session_type = qgetenv("XDG_SESSION_TYPE");
    if (xdg_session_type != "wayland"){
        initPanelDbusGsetting();
        GetsAvailableAreaScreen();
        //qDebug()<<"not wayland";
    } else {
        initset_window_position();
        set_window_position();
        //qDebug()<<"wayland";
    }
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
    <<(int)15;
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
    <<(int)15;
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
    <<(int)15;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}


/**
 * @brief updatewidget::initPanelDbusGsetting
 * 初始化与任务栏gsetting和dbus
 */
void powerwindow::initPanelDbusGsetting()
{
    /* 链接任务栏Dbus接口，获取任务栏高度和位置 */
    m_pServiceInterface = new QDBusInterface(PANEL_DBUS_SERVICE, PANEL_DBUS_PATH, PANEL_DBUS_INTERFACE, QDBusConnection::sessionBus());
    m_pServiceInterface->setTimeout(2147483647);

    connect(m_pPanelSetting, &QGSettings::changed,[=](QString key){
        if ("panelposition" == key | "panelsize" == key) {
             GetsAvailableAreaScreen();
        }
     }
    );
}

void powerwindow::listenPanelChange()
{

    /* 链接任务栏dgsetting接口 */
    if(QGSettings::isSchemaInstalled(UKUI_PANEL_SETTING))
        m_pPanelSetting = new QGSettings(UKUI_PANEL_SETTING);

}

/**
 * @brief updatewidget::connectTaskBarDbus
 * 链接任务栏dbus获取高度的接口
 * @return
 */
int powerwindow::connectTaskBarDbus()
{
    int panelHeight = 46;
    if (m_pPanelSetting != nullptr) {
        QStringList keys = m_pPanelSetting->keys();
        if (keys.contains("panelsize")) {
            panelHeight = m_pPanelSetting->get("panelsize").toInt();
        }
    } else {
        QDBusMessage msg = m_pServiceInterface->call("GetPanelSize", QVariant("Hight"));
        panelHeight = msg.arguments().at(0).toInt();
        return panelHeight;
    }
    return panelHeight;
}

/**
 * @brief updatewidget::getPanelSite
 * 获取任务栏状态位置下上左右
 * @return
 * 返回任务栏位置
 */
int powerwindow::getPanelSite()
{
    int panelPosition = 0;
    if (m_pPanelSetting != nullptr) {
        QStringList keys = m_pPanelSetting->keys();
        if (keys.contains("panelposition")) {
            panelPosition = m_pPanelSetting->get("panelposition").toInt();
        }
    } else {
        QDBusMessage msg = m_pServiceInterface->call("GetPanelPosition", QVariant("Site"));
        panelPosition = msg.arguments().at(0).toInt();
    }
    //qDebug() << "panel所在的位置" << panelPosition;
    return panelPosition;
}

int powerwindow::getScreenGeometry(QString methodName)
{
    int res = 0;
    QDBusMessage message = QDBusMessage::createMethodCall(DBUS_NAME,
                                                          DBUS_PATH,
                                                          DBUS_INTERFACE,
                                                          methodName);
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    if (response.type() == QDBusMessage::ReplyMessage) {
        if (response.arguments().isEmpty() == false) {
            int value = response.arguments().takeFirst().toInt();
            res = value;
            //qDebug() << value;
        }
    } else {
        //qDebug() << methodName << "called failed";
    }
    return res;
}


/**
 * @brief updatewidget::GetsAvailableAreaScreen
 * 获取屏幕的可用区域高度和宽度并设置位置
 */
void powerwindow::GetsAvailableAreaScreen()
{ 
    //如果取不到任务栏的高度,还是优先获取桌面分辨率,可用区域
    if ((0 == connectTaskBarDbus()) && (0 == getPanelSite())) {
        QScreen* pScreen = QGuiApplication::primaryScreen();
        QRect DeskSize = pScreen->availableGeometry();
        m_nScreenWidth = DeskSize.width();                      //桌面分辨率的宽
        m_nScreenHeight = DeskSize.height();                    //桌面分辨率的高
    } else {
        //如果取到任务栏的高度,则取屏幕分辨率的高度
        int h = connectTaskBarDbus();
        QRect screenRect = QGuiApplication::primaryScreen()->geometry();
        m_nScreenWidth = screenRect.width();
        m_nScreenHeight = screenRect.height();
        m_pPeonySite = getPanelSite();
        switch (m_pPeonySite)
        {
            case powerwindow::PanelDown :
                {
                    this->setGeometry(m_nScreenWidth - 364,m_nScreenHeight - h - 198 - deviceNum * 65,300,150-100);
                }
                break;
            case powerwindow::PanelUp:
                {
                    this->setGeometry(m_nScreenWidth - 364, h ,300,150-100);
                }
                break;
            case powerwindow::PanelLeft:
                {
                    this->setGeometry(h,m_nScreenHeight - 190,300,150-100);
                }
                break;
            case powerwindow::PanelRight:
                {
                    this->setGeometry(m_nScreenWidth - 364 - h,m_nScreenHeight - 190,300,150-100);
                }
                break;
            default:
                break;
        }

    }
    //qDebug() << "主屏Width  --> " << m_nScreenWidth;
    //qDebug() << "主屏Height --> " << m_nScreenHeight;



}


int powerwindow::getTaskbarPos(QString str)
{
    QDBusInterface interface( "com.ukui.panel.desktop",
                              "/",
                              "com.ukui.panel.desktop",
                              QDBusConnection::sessionBus() );

    QDBusReply<int> reply = interface.call("GetPanelPosition", str);
    return reply;
}

int powerwindow::getTaskbarHeight(QString str)
{
    QDBusInterface interface( "com.ukui.panel.desktop",
                              "/",
                              "com.ukui.panel.desktop",
                              QDBusConnection::sessionBus() );

    QDBusReply<int> reply = interface.call("GetPanelSize", str);
    return reply;
}

void powerwindow::initset_window_position()
{
    connect(m_pPanelSetting, &QGSettings::changed,[=](QString key){
        if ("panelposition" == key | "panelsize" == key) {
             set_window_position();
        }
     }
    );
}

void powerwindow::set_window_position()
{
#if 0
    QRect rect;
    int availableWidth,totalWidth;
    int availableHeight,totalHeight;
    rect = this->geometry();
    int priX = getScreenGeometry("x");
    int priY = getScreenGeometry("y");
    int priWid = getScreenGeometry("width");
    int priHei = getScreenGeometry("height");
    QRect screenGeometry = qApp->primaryScreen()->geometry();

    availableWidth = priX + priWid;
    availableHeight = priY + priHei;
    totalHeight = screenGeometry.height();
    totalWidth = screenGeometry.width();

    int distance = 4;
    int n = 0;
    int m = 46;

    n = getTaskbarPos("position");
    m = getTaskbarHeight("height");
    if(n == 0){
        //任务栏在下侧
        this->setGeometry(priWid-330-distance,availableHeight-180-m-distance,this->width(),this->height());
    }else if(n == 1){
        //任务栏在上侧
        this->setGeometry(priWid-330-distance,availableHeight-180-distance,this->width(),this->height());
    } else if (n == 2){
        //任务栏在左侧
        this->setGeometry(priWid-330-distance,availableHeight-180-distance,this->width(),this->height());
    } else if (n == 3){
        //任务栏在右侧
        this->setGeometry(availableWidth-330-m-distance,availableHeight-180-distance,this->width(),this->height());
    }
#endif
    QDBusInterface iface("org.ukui.panel",
                         "/panel/position",
                         "org.ukui.panel", QDBusConnection::sessionBus());
    QDBusReply < QVariantList > reply =
        iface.call("GetPrimaryScreenGeometry");
    //qDebug() << reply.value().at(4).toInt();
    QVariantList position_list = reply.value();
    QPoint pt;
    switch (reply.value().at(4).toInt()) {
    case 1:
        this->setGeometry(position_list.at(0).toInt() +
                          position_list.at(2).toInt() - this->width() -
                          MARGIN, position_list.at(1).toInt() + MARGIN,
                          this->width(), this->height());
        break;
    case 2:
        pt = cursor().pos();
        this->setGeometry(position_list.at(0).toInt() + MARGIN,
                          //position_list.at(1).toInt()+reply.value().at(3).toInt()-this->height()-MARGIN,
                          pt.y(), this->width(), this->height());
        break;
    case 3:
        pt = cursor().pos();
        this->setGeometry(position_list.at(2).toInt() - this->width() -
                          MARGIN - getTaskbarHeight("height"),
                          //  position_list.at(1).toInt()+reply.value().at(3).toInt()-this->height()-MARGIN,
                          pt.y(), this->width(), this->height());
        break;
    default:
        this->setGeometry(position_list.at(0).toInt() +
                          position_list.at(2).toInt() - this->width() -
                          MARGIN,
                          position_list.at(1).toInt() +
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


