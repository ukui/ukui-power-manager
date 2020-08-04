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
#include "ukpm_widget.h"
#include "customtype.h"
#include "sys/time.h"
#include <float.h>
#include <QGraphicsDropShadowEffect>
#include <QtMath>
#include <QStyleFactory>

#define GPM_HISTORY_RATE_TEXT			"Rate"
#define GPM_HISTORY_CHARGE_TEXT			"Charge"
#define GPM_HISTORY_TIME_FULL_TEXT		"Time to full"
#define GPM_HISTORY_TIME_EMPTY_TEXT		"Time to empty"

#define GPM_HISTORY_RATE_VALUE			"rate"
#define GPM_HISTORY_CHARGE_VALUE		"charge"
#define GPM_HISTORY_TIME_FULL_VALUE		"time-full"
#define GPM_HISTORY_TIME_EMPTY_VALUE		"time-empty"

#define GPM_HISTORY_MINUTE_TEXT			"10 minutes"
#define GPM_HISTORY_HOUR_TEXT			"2 hours"
#define GPM_HISTORY_HOURS_TEXT			"6 hours"
#define GPM_HISTORY_DAY_TEXT			"1 day"
#define GPM_HISTORY_WEEK_TEXT			"1 week"

#define GPM_HISTORY_MINUTE_VALUE		10*60
#define GPM_HISTORY_HOUR_VALUE			2*60*60
#define GPM_HISTORY_HOURS_VALUE			6*60*60
#define GPM_HISTORY_DAY_VALUE			24*60*60
#define GPM_HISTORY_WEEK_VALUE			7*24*60*60

#define GPM_STATS_CHARGE_DATA_VALUE		"charge-data"
#define GPM_STATS_CHARGE_ACCURACY_VALUE		"charge-accuracy"
#define GPM_STATS_DISCHARGE_DATA_VALUE		"discharge-data"
#define GPM_STATS_DISCHARGE_ACCURACY_VALUE	"discharge-accuracy"


UkpmWidget::UkpmWidget(QWidget *parent)
    : QWidget(parent)
{
    QFile file(":/resource/ui.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QString::fromLatin1(file.readAll());
    qApp->setStyleSheet(styleSheet);
    plotcolor.setNamedColor("#3B3C3F");
    plotcolor.setAlpha(255);
    settings = new QGSettings(GPM_SETTINGS_SCHEMA);
    setupUI();
    getSlots();

}

UkpmWidget::~UkpmWidget()
{
    for(auto iter = listItem.begin(); iter!= listItem.end(); iter++)
    {
        listItem.erase(iter);
        delete iter.value();
    }
}

bool UkpmWidget::set_selected_device(QString name)
{
    if(devices.size() == 0)
    {
        return false;
    }
    QDBusObjectPath objpath(name);
    if(listItem.contains(objpath))
    {
        QListWidgetItem *item = listItem.value(objpath);
        if(dev_item.contains(item))
        {
            current_device = dev_item.value(item)->m_dev;
            listWidget->setItemSelected(item,true);
            listWidget->setCurrentItem(item);
        }
    }
    return  true;
}

QString
UkpmWidget::device_kind_to_localised_text (UpDeviceKind kind, uint number)
{
    Q_UNUSED(number);
    QString text = NULL;
    switch (kind) {
    case UP_DEVICE_KIND_LINE_POWER:
        /* TRANSLATORS: system power cord */
        text = tr ("AC adapter");
        break;
    case UP_DEVICE_KIND_BATTERY:
        /* TRANSLATORS: laptop primary battery */
        text = tr ("Laptop battery");
        break;
    case UP_DEVICE_KIND_UPS:
        /* TRANSLATORS: battery-backed AC power source */
        text = tr ("UPS");
        break;
    case UP_DEVICE_KIND_MONITOR:
        /* TRANSLATORS: a monitor is a device to measure voltage and current */
        text = tr ("Monitor");
        break;
    case UP_DEVICE_KIND_MOUSE:
        /* TRANSLATORS: wireless mice with internal batteries */
        text = tr ("Mouse");
        break;
    case UP_DEVICE_KIND_KEYBOARD:
        /* TRANSLATORS: wireless keyboard with internal battery */
        text = tr ("Keyboard");
        break;
    case UP_DEVICE_KIND_PDA:
        /* TRANSLATORS: portable device */
        text = tr ("PDA");
        break;
    case UP_DEVICE_KIND_PHONE:
        /* TRANSLATORS: cell phone (mobile...) */
        text = tr ("Cell phone");
        break;
    case UP_DEVICE_KIND_MEDIA_PLAYER:
        /* TRANSLATORS: media player, mp3 etc */
        text = tr ("Media player");
        break;
    case UP_DEVICE_KIND_TABLET:
        /* TRANSLATORS: tablet device */
        text = tr ("Tablet");
        break;
    case UP_DEVICE_KIND_COMPUTER:
        /* TRANSLATORS: tablet device */
        text = tr ("Computer");
        break;
    default:
        text = up_device_kind_to_string (kind);
    }
    return text;
}

int UkpmWidget::parseArguments()
{
    int index = 0;
    QStringList arguments = QApplication::arguments();
    if(arguments.size() < 3)
        return index;
    QString dev = arguments.at(2);
    int size = deviceNames.count();
    for(int i = 0; i< size; i++)
        if(deviceNames.at(i).path() == dev)
            index = i;
    return index;
}

void UkpmWidget::getProperty(QString path,DEV& dev)
{

    QDBusMessage msg = QDBusMessage::createMethodCall(DBUS_SERVICE,path,
            DBUS_INTERFACE,"GetAll");
    msg << DBUS_INTERFACE_PARAM;
    QDBusMessage res = QDBusConnection::systemBus().call(msg);

    if(res.type() == QDBusMessage::ReplyMessage)
    {
        const QDBusArgument &dbusArg = res.arguments().at(0).value<QDBusArgument>();
        QMap<QString,QVariant> map;
        dbusArg >> map;

        dev.kind = (UpDeviceKind)map.value(QString("Type")).toInt();
        dev.Type = device_kind_to_localised_text ((UpDeviceKind)map.value(QString("Type")).toInt(),1);
        dev.Model = map.value(QString("Model")).toString();
        dev.Device = map.value(QString("NativePath")).toString();
        dev.Vendor = map.value(QString("Vendor")).toString();
        dev.Capacity = QString::number(map.value(QString("Capacity")).toDouble(), 'f', 1) + "%";
        dev.Energy = QString::number(map.value(QString("Energy")).toDouble(), 'f', 1)+ " Wh";
        dev.EnergyEmpty= QString::number(map.value(QString("EnergyEmpty")).toDouble(), 'f', 1)+ " Wh";
        dev.EnergyFull = QString::number(map.value(QString("EnergyFull")).toDouble(), 'f', 1)+ " Wh";
        dev.EnergyFullDesign = QString::number(map.value(QString("EnergyFullDesign")).toDouble(), 'f', 1) + " Wh";
        dev.EnergyRate = QString::number(map.value(QString("EnergyRate")).toDouble(), 'f', 1) + " W";
        dev.IsPresent = boolToString(map.value(QString("IsPresent")).toBool());
        dev.IsRechargeable = boolToString(map.value(QString("IsRechargeable")).toBool());
        dev.PowerSupply = boolToString(map.value(QString("PowerSupply")).toBool());
        dev.Percentage = QString::number(map.value(QString("Percentage")).toDouble(), 'f', 1)+"%";
        dev.Online = boolToString(map.value(QString("Online")).toBool());

        dev.update_time = QString::number(map.value(QString("UpdateTime")).toLongLong());

        int flag = map.value(QString("State")).toLongLong();
        switch (flag) {
        case 1:
            dev.State = tr("Charging");
            break;
        case 2:
            dev.State = tr("Discharging");
            break;
        case 3:
            dev.State = tr("Empty");
            break;
        case 4:
            dev.State = tr("Charged");
            break;
        case 5:
            dev.State = tr("PendingCharge");
            break;
        case 6:
            dev.State = tr("PendingDischarge");
            break;
        default:
            break;
        }
        calcTime(dev.TimeToEmpty, map.value(QString("TimeToEmpty")).toLongLong());
        calcTime(dev.TimeToFull, map.value(QString("TimeToFull")).toLongLong());
        dev.Voltage = QString::number(map.value(QString("Voltage")).toDouble(), 'f', 1) + " V";

    }
}

void UkpmWidget::getDevices()
{
    QListWidgetItem *item;
    QString kind,vendor,model,label;
    int kindEnum = 0;
    QDBusMessage msg = QDBusMessage::createMethodCall(DBUS_SERVICE,DBUS_OBJECT,
            "org.freedesktop.UPower","EnumerateDevices");
    QDBusMessage res = QDBusConnection::systemBus().call(msg);

    if(res.type() == QDBusMessage::ReplyMessage)
    {
        const QDBusArgument &dbusArg = res.arguments().at(0).value<QDBusArgument>();
        dbusArg >> deviceNames;
    }
    else {
    }
    int len = deviceNames.size();
    for(int i = 0; i < len; i++)
    {
        if(deviceNames.at(i).path().contains("battery"))
            batterySvr = deviceNames.at(i).path();
        else if(deviceNames.at(i).path().contains("line_power"))
            acSvr = deviceNames.at(i).path();
        QDBusMessage msg = QDBusMessage::createMethodCall(DBUS_SERVICE,deviceNames.at(i).path(),
                DBUS_INTERFACE,"GetAll");
        msg << DBUS_INTERFACE_PARAM;
        QDBusMessage res = QDBusConnection::systemBus().call(msg);

        if(res.type() == QDBusMessage::ReplyMessage)
        {
            const QDBusArgument &dbusArg = res.arguments().at(0).value<QDBusArgument>();
            QMap<QString,QVariant> map;
            dbusArg >> map;
            kind = map.value(QString("kind")).toString();
            if(kind.length() ==0)
                kind = map.value(QString("Type")).toString();
            kindEnum = kind.toInt();
            QString icon = up_device_kind_to_string((UpDeviceKind)kindEnum);
            vendor = map.value(QString("Vendor")).toString();
            model = map.value(QString("Model")).toString();
            if(vendor.length() != 0 && model.length() != 0)
                label = vendor + " " + model;
            else
                label =device_kind_to_localised_text((UpDeviceKind)kindEnum,1);

            if(kindEnum == UP_DEVICE_KIND_LINE_POWER || kindEnum == UP_DEVICE_KIND_BATTERY || kindEnum == UP_DEVICE_KIND_COMPUTER)
            {
                DeviceWidget *dw = new DeviceWidget;
                dw->setAttribute(Qt::WA_DeleteOnClose);
                dw->set_device_icon(":/resource/icon/"+icon+".png");
                dw->set_device_text(false,label);
//                item = new QListWidgetItem(QIcon(":/resource/icon/"+icon+".png"),label);
                item = new QListWidgetItem;
                item->setSizeHint(QSize(150,36));
                dw->setFixedSize(item->sizeHint());
                listItem.insert(deviceNames.at(i),item);
                listWidget->addItem(item);
                listWidget->setItemWidget(item,dw);
            }
            else
                return;

            DEVICE *dev = new DEVICE(this);
            dev->m_dev->kind = (UpDeviceKind)kindEnum;
            dev->m_dev->path = deviceNames.at(i).path();
            dev->m_dev->hasHistory = map.value(QString("HasHistory")).toBool();
            dev->m_dev->hasStat = map.value(QString("HasStatistics")).toBool();
            dev->m_dev->Type = up_device_kind_to_string ((UpDeviceKind)map.value(QString("Type")).toInt());
            dev->m_dev->Model = map.value(QString("Model")).toString();
            dev->m_dev->Device = map.value(QString("NativePath")).toString();
            dev->m_dev->Vendor = map.value(QString("Vendor")).toString();
            dev->m_dev->Capacity = QString::number(map.value(QString("Capacity")).toDouble(), 'f', 1) + "%";
            dev->m_dev->Energy = QString::number(map.value(QString("Energy")).toDouble(), 'f', 1)+ " Wh";
            dev->m_dev->EnergyEmpty= QString::number(map.value(QString("EnergyEmpty")).toDouble(), 'f', 1)+ " Wh";
            dev->m_dev->EnergyFull = QString::number(map.value(QString("EnergyFull")).toDouble(), 'f', 1)+ " Wh";
            dev->m_dev->EnergyFullDesign = QString::number(map.value(QString("EnergyFullDesign")).toDouble(), 'f', 1) + " Wh";
            dev->m_dev->EnergyRate = QString::number(map.value(QString("EnergyRate")).toDouble(), 'f', 1) + " W";
            dev->m_dev->IsPresent = boolToString(map.value(QString("IsPresent")).toBool());
            dev->m_dev->IsRechargeable = boolToString(map.value(QString("IsRechargeable")).toBool());
            dev->m_dev->PowerSupply = boolToString(map.value(QString("PowerSupply")).toBool());
            dev->m_dev->Percentage = QString::number(map.value(QString("Percentage")).toDouble(), 'f', 1)+"%";
            dev->m_dev->Online = boolToString(map.value(QString("Online")).toBool());


            int flag = map.value(QString("State")).toLongLong();
            switch (flag) {
            case 1:
                dev->m_dev->State = tr("Charging");
                break;
            case 2:
                dev->m_dev->State = tr("Discharging");
                break;
            case 3:
                dev->m_dev->State = tr("Empty");
                break;
            case 4:
                dev->m_dev->State = tr("Charged");
                break;
            case 5:
                dev->m_dev->State = tr("PendingCharge");
                break;
            case 6:
                dev->m_dev->State = tr("PendingDischarge");
                break;
            default:
                break;
            }
            calcTime(dev->m_dev->TimeToEmpty, map.value(QString("TimeToEmpty")).toLongLong());
            calcTime(dev->m_dev->TimeToFull, map.value(QString("TimeToFull")).toLongLong());
            dev->m_dev->Voltage = QString::number(map.value(QString("Voltage")).toDouble(), 'f', 1) + " V";

            devices.push_back(dev);
            dev_item.insert(item,dev);
        }
    }


}

void UkpmWidget::calcTime(QString &attr, uint time)
{
    if(time < 60)
    {
        attr = getSufix(time,'s');
        return;
    }
    time /= 60;
    if(time < 60)
    {
        attr = getSufix(time,'m');
        return;
    }
    qreal hour = time / 60.0;
    {
        attr = getSufix(hour,'h');
        return;
    }
}

void UkpmWidget::onShow()
{
    QDesktopWidget *deskdop = QApplication::desktop();
    move((deskdop->width() - this->width())/2, (deskdop->height() - this->height())/2);
    this->show();
}

QString UkpmWidget::getSufix(uint tim, char c)
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

QString UkpmWidget::up_device_kind_to_string (UpDeviceKind type_enum)
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

void UkpmWidget::onActivatedIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    {
        QPoint post;
        post.setX(-menu->sizeHint().width()/2 + QCursor::pos().x());
        post.setY(QCursor::pos().y());
        menu->exec(post);
    }
        break;

    default:
        break;
    }

}

void UkpmWidget::addListRow(QString attr,QString value)
{
    QList<QStandardItem *> list;
    list << new QStandardItem(attr) << new QStandardItem(value);
    model->insertRow(model->rowCount(),list);
}

/**
 * ukpm_update_info_page_details:
 **/

void UkpmWidget::
ukpm_update_info_page_details (DEV* device)
{
    if(current_device == NULL)
        return;
    DEV dev;
    getProperty(device->path,dev);
    /* get a human readable time */
    struct timeval tv;
    uint tim;
    gettimeofday(&tv,NULL);
    QString device_path = device->path.split("/").last();

    if(model==NULL)
        return;
    model->clear();
    QStringList header;
    header << tr("Attribute") << tr("Value");
//    model->setHorizontalHeaderLabels(header);
    addListRow(tr("Attribute"),tr("Value"));

    addListRow(tr("Device"),device_path);
    /* TRANSLATORS: the device ID of the current device, e.g. "battery0" */
    addListRow(tr("Type"),dev.Type);

    if (!dev.Vendor.isEmpty())
        addListRow (tr("Vendor"), dev.Vendor);
    if (!dev.Model.isEmpty())
        addListRow (tr("Model"), dev.Model);
    if (!dev.serial.isEmpty())
        addListRow (tr("Serial number"), dev.serial);

    addListRow (tr("Supply"), dev.PowerSupply);

    if (dev.kind == UP_DEVICE_KIND_BATTERY ||
        dev.kind == UP_DEVICE_KIND_MOUSE ||
        dev.kind == UP_DEVICE_KIND_KEYBOARD)
    {
        tim = tv.tv_sec - dev.update_time.toLong();
        dev.Refresh = getSufix(tim,'s');

        addListRow (tr("Refreshed"), dev.Refresh);
    }

    if (dev.kind == UP_DEVICE_KIND_BATTERY ||
        dev.kind == UP_DEVICE_KIND_MOUSE ||
        dev.kind == UP_DEVICE_KIND_KEYBOARD ||
        dev.kind == UP_DEVICE_KIND_UPS) {

        addListRow (tr("Present"), dev.IsPresent);
    }
    if (dev.kind == UP_DEVICE_KIND_BATTERY ||
        dev.kind == UP_DEVICE_KIND_MOUSE ||
        dev.kind == UP_DEVICE_KIND_KEYBOARD) {

        addListRow (tr("Rechargeable"), dev.IsRechargeable);

    }
    if (dev.kind == UP_DEVICE_KIND_BATTERY ||
        dev.kind == UP_DEVICE_KIND_MOUSE ||
        dev.kind == UP_DEVICE_KIND_KEYBOARD) {

        addListRow (tr("State"), dev.State);

    }
    if (dev.kind == UP_DEVICE_KIND_BATTERY) {

        addListRow (tr("Energy"), dev.Energy);
        //addListRow (tr("Energy when empty"), dev.EnergyEmpty);
        addListRow (tr("Energy when full"), dev.EnergyFull);
        addListRow (tr("Energy (design)"), dev.EnergyFullDesign);
    }
    if (dev.kind == UP_DEVICE_KIND_BATTERY ||
        dev.kind == UP_DEVICE_KIND_MONITOR) {

        addListRow (tr("Rate"), dev.EnergyRate);
    }
    if (dev.kind == UP_DEVICE_KIND_UPS ||
        dev.kind == UP_DEVICE_KIND_BATTERY ||
        dev.kind == UP_DEVICE_KIND_MONITOR) {

        addListRow (tr("Voltage"), dev.Voltage);
    }
    if (dev.kind == UP_DEVICE_KIND_BATTERY ||
        dev.kind == UP_DEVICE_KIND_UPS) {
        if (dev.TimeToFull >= 0) {
            addListRow (tr("Time to full"), dev.TimeToFull);

        }
        if (dev.TimeToEmpty >= 0) {
            addListRow (tr("Time to empty"), dev.TimeToEmpty);

        }
    }
    if (dev.kind == UP_DEVICE_KIND_BATTERY ||
        dev.kind == UP_DEVICE_KIND_MOUSE ||
        dev.kind == UP_DEVICE_KIND_KEYBOARD ||
        dev.kind == UP_DEVICE_KIND_UPS) {

        addListRow (tr("Percentage"), dev.Percentage);

    }
    if (dev.kind == UP_DEVICE_KIND_BATTERY) {

        addListRow (tr("Capacity"), dev.Capacity);
    }

    if (dev.kind == UP_DEVICE_KIND_LINE_POWER) {
        addListRow (tr("Online"), dev.Online);
    }
}

void UkpmWidget::ukpm_update_info_data (DEV* device)
{
    if(current_device == NULL)
        return;
    int page;
    bool has_history;
    bool has_statistics;
    has_history = device->hasHistory;
    has_statistics = device->hasStat;
    int index;
    index = tab_widget->indexOf(his_widget);
    if (has_history)
    {
        if(index == -1)
            tab_widget->addTab(his_widget,tr(" history "));
    }
    else
    {
        if(-1 != index)
            tab_widget->removeTab(index);
    }
    index = tab_widget->indexOf(stat_widget);
    if (has_statistics)
    {
        if(-1 == index)
            tab_widget->addTab(stat_widget,tr(" statistics "));
    }
    else
    {
        if(-1 != index)
            tab_widget->removeTab(index);
    }

    page = tab_widget->currentIndex();
    index_old = page;
    ukpm_update_info_data_page (device, page);
    return;
}

void UkpmWidget::ukpm_update_info_page_history (DEV* device)
{
    Q_UNUSED(device);
    if(current_device == NULL || !current_device->hasHistory)
        return;
    int index = spanCombox->currentIndex();
    if(index == TENM)
    {
        timeSpan = 600;
    }
    else if(index == TWOH)
    {
        timeSpan = 2*60*60;
    }
    else if(index == SIXH)
    {
        timeSpan = 6*60*60;
    }
    else if(index == ONED)
    {
        timeSpan = 24*60*60;
    }
    else if(index == ONEW)
    {
        timeSpan = 7*24*60*60;
    }

    QString type;
    index = typeCombox->currentIndex();
    if (index==RATE) {
        current_device->type_x = GPM_GRAPH_WIDGET_TYPE_TIME;
        current_device->type_y = GPM_GRAPH_WIDGET_TYPE_PERCENTAGE;
        current_device->autorange_x = false;
        current_device->start_x = -timeSpan;
        current_device->stop_x = 0;
        current_device->autorange_y = false;
        current_device->start_y = 0;
        current_device->stop_y = 100;
        type = "rate";

    } else if (index==VOLUME) {

        current_device->type_x = GPM_GRAPH_WIDGET_TYPE_TIME;
        current_device->type_y = GPM_GRAPH_WIDGET_TYPE_POWER;
        current_device->autorange_x = false;
        current_device->start_x = -timeSpan;
        current_device->stop_x = 0;
        current_device->autorange_y = true;
        type = "charge";

    } else if (index==CHARGE_DURATION) {

        current_device->type_x = GPM_GRAPH_WIDGET_TYPE_TIME;
        current_device->type_y = GPM_GRAPH_WIDGET_TYPE_TIME;
        current_device->autorange_x = false;
        current_device->start_x = -timeSpan;
        current_device->stop_x = 0;
        current_device->autorange_y = true;
        type = "time-full";

    }else if (index==DISCHARGING_DURATION) {

        current_device->type_x = GPM_GRAPH_WIDGET_TYPE_TIME;
        current_device->type_y = GPM_GRAPH_WIDGET_TYPE_TIME;
        current_device->autorange_x = false;
        current_device->start_x = -timeSpan;
        current_device->stop_x = 0;
        current_device->autorange_y = true;
        type = "time-empty";
    }
    draw_history_graph(type);
}

int UkpmWidget::calculate_up_number(float value, int div)
{
    float res;
    if(fabs(value) < 0.01)
        return 0;
    if(div == 0)
        return 0;
    res = (float) value / (float) div;
    res = ceil(res);
    res *= div;
    return (int)res;
}

int UkpmWidget::calculate_down_number(float value, int div)
{
    float res;
    if(fabs(value) < 0.01)
        return 0;
    if(div == 0)
        return 0;
    res = (float)value / (float)div;
    res = floor(res);
    res *= div;
    return (int)res;
}

void UkpmWidget::draw_stats_graph(QString type)
{
    int start_x = 0;
    float max_y = FLT_MIN;
    float min_y = FLT_MAX;
    int starty = 0;
    int stopy = 0;

    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.UPower",current_device->path,
            "org.freedesktop.UPower.Device","GetStatistics");
    QList<QPointF> list;
    list.clear();

    msg << type;
    QDBusMessage res = QDBusConnection::systemBus().call(msg);
    if(res.type() == QDBusMessage::ReplyMessage)
    {
        QDBusArgument dbusArg = res.arguments().at(0).value<QDBusArgument>();
        dbusArg >> list;
    }
    else {

    }

    QPointF pit;
    QList<QPointF> data;
    int list_size = list.size();
    int index = sumTypeCombox->currentIndex();
    if(index == CHARGE)
    {
        if(list_size > 0)
        {
            foreach(pit, list)
            {
                data.append(QPointF(start_x++,pit.x()));
                if(max_y < (pit.x()))
                    max_y = (pit.x());
                if(min_y > (pit.x()))
                    min_y = pit.x();
            }

            if(max_y - min_y < 0.0001)
            {
                max_y++;
                min_y--;
            }
            starty = calculate_down_number(min_y,1);
            stopy = calculate_up_number(max_y,1);
            if(fabs(stopy) > fabs(starty))
                starty = -stopy;
            else
                stopy = -starty;

            if(starty == stopy)
            {
                starty = -1;
                stopy = 1;
            }
        }
        else
        {
            starty = -1;
            stopy = 1;
        }
        sumSeries->replace(data);
        sumSpline->replace(data);
        y->setTitleText(tr("adjust factor"));
        y->setRange(starty,stopy);
        y->setLabelFormat("%.1f");
        x->setTitleText(tr("battery power"));
        x->setRange(0,100);
        x->setLabelFormat("%d%");
        x->setTickCount(10);
        y->setTickCount(10);

    }
    else if(index == CHARGE_ACCURENCY)
    {
        if(list_size > 0)
        {
            foreach(pit, list)
            {
                data.append(QPointF(start_x++,pit.y()));
                if(max_y < (pit.y()))
                    max_y = (pit.y());
                if(min_y > pit.y())
                    min_y = pit.y();
            }
            if(max_y - min_y < 0.0001)
            {
                max_y++;
                min_y--;
            }
            starty = calculate_down_number(min_y,10);
            stopy = calculate_up_number(max_y,10);
            stopy += 10;
            if(stopy >= 90)
                stopy = 100;
            if(starty >0 && starty <= 10)
                starty = 0;
        }
        else
        {
            starty = 0;
            stopy = 100;
        }

        sumSeries->replace(data);
        sumSpline->replace(data);
        y->setTitleText(tr("Predict Accurency"));
        y->setRange(starty,stopy);
        y->setLabelFormat("%d%");
        x->setTitleText(tr("battery power"));
        x->setRange(0,100);
        x->setLabelFormat("%d%");
        x->setTickCount(10);
        y->setTickCount(10);
    }
    else if(index == DISCHARGING)
    {
        if(list_size > 0)
        {
            foreach(pit, list)
            {
                data.append(QPointF(start_x++,pit.x()));
                if(max_y < (pit.x()))
                    max_y = (pit.x());
                if(min_y > (pit.x()))
                    min_y = pit.x();
            }
            if(max_y - min_y < 0.0001)
            {
                max_y++;
                min_y--;
            }
            starty = calculate_down_number(min_y,1);
            stopy = calculate_up_number(max_y,1);
            if(fabs(stopy) > fabs(starty))
                starty = -stopy;
            else
                stopy = -starty;

            if(starty == stopy)
            {
                starty = -1;
                stopy = 1;
            }
        }
        else
        {
            starty = -1;
            stopy = 1;
        }
        sumSeries->replace(data);
        sumSpline->replace(data);
        y->setTitleText(tr("adjust factor"));
        y->setRange(starty,stopy);
        y->setLabelFormat("%.1f");
        x->setTitleText(tr("battery power"));
        x->setRange(0,100);
        x->setLabelFormat("%d%");
        x->setTickCount(10);
        y->setTickCount(10);
    }
    else if(index == DISCHARGING_ACCURENCY)
    {
        if(list_size > 0)
        {
            foreach(pit, list)
            {
                data.append(QPointF(start_x++,pit.y()));
                if(max_y < (pit.y()))
                    max_y = (pit.y());
                if(min_y > pit.y())
                    min_y = pit.y();
            }
            if(max_y - min_y < 0.0001)
            {
                max_y++;
                min_y--;
            }
            starty = calculate_down_number(min_y,10);
            stopy = calculate_up_number(max_y,10);
            if(stopy >= 90)
                stopy = 100;
            if(starty >0 && starty <= 10)
                starty = 0;
        }
        else
        {
            starty = 0;
            stopy = 100;
        }
        sumSeries->replace(data);
        sumSpline->replace(data);
        y->setTitleText(tr("Predict Accurency"));
//        y->setRange(0,100);
        y->setRange(starty,stopy);
        y->setLabelFormat("%d%");
        y->setTickCount(10);

        x->setTitleText(tr("battery power"));
        x->setRange(0,100);
        x->setLabelFormat("%d%");
        x->setTickCount(10);
    }
}

void UkpmWidget::ukpm_set_choice_sum()
{
    bool checked;

    sumChart->addSeries(sumSpline);
    sumSpline->attachAxis(x);//连接数据集与
    sumSpline->attachAxis(y);//连接数据集与
    sumChart->addSeries(sumSeries);
    sumSeries->attachAxis(x);//连接数据集与
    sumSeries->attachAxis(y);//连接数据集与

    QString stats_type = settings->get(GPM_SETTINGS_INFO_STATS_TYPE).toString();
    if(stats_type == NULL)
        stats_type = GPM_STATS_CHARGE_DATA_VALUE;
    if (QString::compare( stats_type, GPM_STATS_CHARGE_DATA_VALUE) == 0)
        sumTypeCombox->setCurrentIndex(0);
    else if (QString::compare(stats_type, GPM_STATS_CHARGE_ACCURACY_VALUE) == 0)
        sumTypeCombox->setCurrentIndex(1);
    else if (QString::compare (stats_type, GPM_STATS_DISCHARGE_DATA_VALUE) == 0)
        sumTypeCombox->setCurrentIndex(2);
    else
        sumTypeCombox->setCurrentIndex(3);

    checked = settings->get(GPM_SETTINGS_INFO_STATS_GRAPH_POINTS).toBool();
    if(checked)
    {
        sum_data_btn->setStyleSheet("QPushButton {color:white; background:#3D6BE5; border-radius:4px}");
        sum_line_btn->setStyleSheet("QPushButton {color:black; background:#E7E7E7; border-radius:4px}");
        sumSpline->setVisible(true);
        sumSeries->setVisible(false);

    }
    else
    {
        sum_line_btn->setStyleSheet("QPushButton {color:white; background:#3D6BE5; border-radius:4px}");
        sum_data_btn->setStyleSheet("QPushButton {color:black; background:#E7E7E7; border-radius:4px}");
        sumSeries->setVisible(true);
        sumSpline->setVisible(false);

    }
}

void UkpmWidget::draw_history_graph(QString type)
{
    QStringList labels;
    float max_y = 0;
    float min_y = 0;
    int starty = 0;
    int stopy = 0;

    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.UPower",current_device->path,
            "org.freedesktop.UPower.Device","GetHistory");
    QList<QPointF> list;
    list.clear();
    QList<StructUdu> listQDBus;
    QVariant variant;
    uint size;
    QPointF temp;
    QDBusArgument argument;
    struct timeval tv;
    gettimeofday(&tv,NULL);

    resolution = 150;
    msg << type << timeSpan << resolution;
    QDBusMessage res = QDBusConnection::systemBus().call(msg);
    if(res.type() == QDBusMessage::ReplyMessage)
    {
        variant = res.arguments().at(0);
        argument = variant.value<QDBusArgument>();
        argument >> listQDBus;
        size = listQDBus.size();
        for(uint i = 0; i< size; i++)
        {
            if(listQDBus[i].state == 0)
                continue;
            temp.setX(tv.tv_sec - listQDBus[i].time);
            temp.setY(listQDBus[i].value);
            list.append(temp);
        }
    }
    else {
    }

    int index = typeCombox->currentIndex();
    //y
    if (index==RATE) {
        labels = axisY->categoriesLabels();
        foreach (QString str, labels) {
            axisY->remove(str);
        }

        int size = list.size();
        if(size > 0)
        {
            min_y = max_y = list.at(0).y();
            for(int i = 1; i< size; i++)
            {
                double tmp = list.at(i).y();
                if(min_y > tmp)
                    min_y = tmp;
                if(max_y < tmp)
                    max_y = tmp;
            }
            if(max_y - min_y < 0.0001)
            {
                max_y ++;
            }

            starty = calculate_down_number(min_y,10);
            stopy = calculate_up_number(max_y,10);

            if(starty == stopy)
            {
                starty = 0;
                stopy = 10;
            }
        }
        else
        {
            starty = 0;
            stopy = 10;
        }
        axisY->setMin(starty);
        axisY->setMax(stopy);
        axisY->setStartValue(starty);
        axisY->setTitleText(tr("Rate"));
//        axisY->setLabelFormat("%6.1fw");
        axisY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
        for(int i = 0; i < 11; i++)
        {
            QString str;
            str.sprintf("%9.1fw",starty+i*(stopy-starty)/10.0);
//            str_new = QString("%1").arg(str,6,QLatin1Char(' '));SSSSSS
            axisY->append(str,starty+i*(stopy-starty)/10.0);
        }

        hisSeries->replace(list);
        hisSpline->replace(list);

    } else if (index==VOLUME) {

        axisY->setTitleText(tr("Charge"));

        labels = axisY->categoriesLabels();
        foreach (QString str, labels) {
            axisY->remove(str);
        }
        axisY->setMin(0);
        axisY->setMax(100);
        axisY->setStartValue(0);
        for(int i = 0; i < 11; i++)
        {
            QString str;
            str.sprintf("%d%%",i*10);
            axisY->append(str,i*10);
        }
        axisY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
        hisSeries->replace(list);
        hisSpline->replace(list);

    } else if (index==CHARGE_DURATION) {

        axisY->setTitleText(tr("Predict Time"));
        labels = axisY->categoriesLabels();
        foreach (QString str, labels) {
            axisY->remove(str);
        }
        int size = list.size();
        if(size > 0)
        {
            min_y = max_y = list.at(0).y();
            for(int i = 0; i< size; i++)
            {
                float tmp = list.at(i).y();
                if(min_y > tmp)
                    min_y = tmp;
                if(max_y < tmp)
                    max_y = tmp;
            }
            if(max_y - min_y < 0.0001)
            {
                max_y++;
            }
            starty = calculate_down_number(min_y,600);
            stopy = calculate_up_number(max_y,600);
        }
        else
        {
            starty = 0;
            stopy = 60*10;
        }

        axisY->setMin(starty);
        axisY->setMax(stopy);
        axisY->setStartValue(starty);
        for(int i = 0; i < 11; i++)
        {
            axisY->append(getWidgetAxis(starty+i*(stopy-starty)/10),starty+i*(stopy-starty)/10);
        }
        axisY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
        hisSeries->replace(list);
        hisSpline->replace(list);

    }else if (index==DISCHARGING_DURATION) {

        axisY->setTitleText(tr("Predict Time"));
        labels = axisY->categoriesLabels();
        foreach (QString str, labels) {
            axisY->remove(str);
        }

        int size = list.size();
        if(size > 0)
        {
            min_y = max_y = list.at(0).y();
            for(int i = 0; i< size; i++)
            {
                float tmp = list.at(i).y();
                if(min_y > tmp)
                    min_y = tmp;
                if(max_y < tmp)
                    max_y = tmp;
            }
            if(max_y - min_y < 0.0001)
            {
                max_y++;
            }
            starty = calculate_down_number(min_y,600);
            stopy = calculate_up_number(max_y,600);
        }
        else
        {
            starty = 0;
            stopy = 60*10;
        }


        axisY->setMin(starty);
        axisY->setMax(stopy);
        axisY->setStartValue(starty);
        for(int i = 0; i < 11; i++)
        {
            axisY->append(getWidgetAxis(starty+i*(stopy-starty)/10),starty+i*(stopy-starty)/10);
        }

        axisY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
        hisSeries->replace(list);
        hisSpline->replace(list);
    }
    //x
    labels = xtime->categoriesLabels();
    foreach (QString str, labels) {
        xtime->remove(str);
    }
    xtime->setMin(0);
    xtime->setMax(timeSpan);
    xtime->setStartValue(0);
    for(int i = 0; i < 11; i++)
    {
        xtime->append(getWidgetAxis(i*timeSpan/10),i*timeSpan/10);
    }
    xtime->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
//    settings->setInt(GPM_SETTINGS_INFO_HISTORY_TIME,timeSpan);
//    hisStack->setCurrentIndex(1);

}

void UkpmWidget::ukpm_set_choice_history()
{
    bool checked;
    hisChart->addSeries(hisSpline);
    hisSpline->attachAxis(xtime);//连接数据集与
    hisSpline->attachAxis(axisY);//连接数据集与
    hisChart->addSeries(hisSeries);
    hisSeries->attachAxis(xtime);//连接数据集与
    hisSeries->attachAxis(axisY);//连接数据集与

    QString history_type = settings->get(GPM_SETTINGS_INFO_HISTORY_TYPE).toString();
    int history_time = settings->get(GPM_SETTINGS_INFO_HISTORY_TIME).toInt();
    if (history_type == NULL)
        history_type = GPM_HISTORY_CHARGE_VALUE;
    if (history_time == 0)
        history_time = GPM_HISTORY_HOUR_VALUE;

    if (QString::compare (history_type, GPM_HISTORY_RATE_VALUE) == 0)
        typeCombox->setCurrentIndex(0);
    else
        typeCombox->setCurrentIndex(1);

    if (history_time == GPM_HISTORY_MINUTE_VALUE)
        spanCombox->setCurrentIndex(0);
    else if (history_time == GPM_HISTORY_HOUR_VALUE)
        spanCombox->setCurrentIndex(1);
    else
        spanCombox->setCurrentIndex(2);

    checked = settings->get(GPM_SETTINGS_INFO_HISTORY_GRAPH_POINTS).toBool();
    if(checked)
    {
        his_data_btn->setStyleSheet("QPushButton {color:white; background:#3D6BE5; border-radius:4px}");
        his_line_btn->setStyleSheet("QPushButton {color:black; background:#E7E7E7; border-radius:4px}");
        hisSpline->setVisible(true);
        hisSeries->setVisible(false);
    }
    else
    {
        his_data_btn->setStyleSheet("QPushButton {color:black; background:#E7E7E7; border-radius:4px}");
        his_line_btn->setStyleSheet("QPushButton {color:white; background:#3D6BE5; border-radius:4px}");

        hisSpline->setVisible(false);
        hisSeries->setVisible(true);
    }

}
void UkpmWidget::ukpm_update_info_data_page (DEV* device, int page)
{
    if (page == 0)
        ukpm_update_info_page_details (device);
    else if (page == 1)
        ukpm_update_info_page_history (device);
    else if (page == 2)
        ukpm_update_info_page_stats (device);
}

void UkpmWidget::ukpm_update_info_page_stats (DEV* device)
{
    Q_UNUSED(device);
    if(current_device == NULL || !current_device->hasStat)
        return;
    QString type;
    int index = sumTypeCombox->currentIndex();

    if(index == CHARGE)
    {
        type = "charging";
    }
    else if(index == CHARGE_ACCURENCY)
    {
        type = "charging";
    }
    else if(index == DISCHARGING)
    {
        type = "discharging";
    }
    else if(index == DISCHARGING_ACCURENCY)
    {
        type = "discharging";
    }
    draw_stats_graph (type);
//    ukpm_set_choice_sum();

}

void UkpmWidget::setSumTab()
{
    stat_widget = new QWidget();
    QLabel *graphicType = new QLabel(tr("graphic type:"),stat_widget);
    graphicType->setObjectName("m_sumGraphicType");
    sumTypeCombox = new QComboBox(stat_widget);
    sumTypeCombox->addItems(QStringList()<<tr("charge")<<tr("charge-accurency")<<tr("discharge")<<tr("discharge-accurency"));

    QListView * listView = new QListView(sumTypeCombox);
    sumTypeCombox->setView(listView);

    sum_line_btn = new QPushButton(tr("show line"),stat_widget);
    sum_data_btn = new QPushButton(tr("show points"),stat_widget);
    sum_line_btn->setObjectName("sum_line_btn");
    sum_data_btn->setObjectName("sum_data_btn");
    sum_line_btn->setFixedSize(100,32);
    sum_data_btn->setFixedSize(100,32);
    sum_line_btn->setCheckable(true);
    sum_data_btn->setCheckable(true);
    sum_group = new QButtonGroup;
    sum_group->addButton(sum_line_btn);
    sum_group->addButton(sum_data_btn);
    sum_group->setExclusive(true);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    QFormLayout *topFormLayout = new QFormLayout;

    graphicType->setFixedHeight(36);
    sumTypeCombox->setFixedWidth(200);
    sumTypeCombox->setFixedHeight(36);
    topFormLayout->addRow(graphicType,sumTypeCombox);
    topFormLayout->setSpacing(0);
    topFormLayout->setContentsMargins(0,0,0,0);

    sumChart = new QChart;
    x = new QValueAxis;
    y = new QValueAxis;

    sumSeries = new QLineSeries();
    sumSpline = new QScatterSeries();
    sumSpline->setMarkerSize(6);
    sumSpline->setBrush(QBrush(QColor("#3D6BE5")));
    sumSpline->setBorderColor(QColor("#3D6BE5"));
    QPen pen(QColor("#2ac4a1"),1);
    sumSeries->setPen(pen);

    sumChart->setAxisX(x);
    sumChart->setAxisY(y);

    QFont font = x->labelsFont();
    font.setPixelSize(12);
    x->setLabelsFont(font);
    y->setLabelsFont(font);
    font.setBold(true);
    x->setTitleFont(font);
    y->setTitleFont(font);
    x->setGridLinePen(QPen(QColor("#343537"),1));
    y->setGridLinePen(QPen(QColor("#343537"),1));
    x->setLineVisible(false);
    y->setLineVisible(false);

    sumChart->legend()->hide();

    sumChart->setPlotAreaBackgroundBrush(plotcolor);
    sumChart->setPlotAreaBackgroundVisible(true);

    sumChartView = new QChartView(sumChart);
    sumChartView->setRenderHint(QPainter::Antialiasing);
    sumChartView->setContentsMargins(0,0,0,0);
    sumChart->setContentsMargins(0,10,0,0);
    sumChart->setMargins(QMargins(0,0,0,0));

    QVBoxLayout *botchild = new QVBoxLayout;
    bottomLayout->addWidget(sum_line_btn,1);
    bottomLayout->addWidget(sum_data_btn,1);
    bottomLayout->addLayout(botchild,4);
    bottomLayout->setSpacing(16);

    QVBoxLayout *vLayout = new QVBoxLayout;

    vLayout->addLayout(topFormLayout);
    vLayout->addWidget(sumChartView);
    vLayout->addLayout(bottomLayout);
    vLayout->setContentsMargins(0,20,0,32);
    vLayout->setSpacing(0);
    stat_widget->setLayout(vLayout);
    stat_widget->hide();
    ukpm_set_choice_sum();
}
void UkpmWidget::showHisDataPoint(bool flag)
{
    settings->set(GPM_SETTINGS_INFO_HISTORY_GRAPH_POINTS,flag);
    QList<QAbstractSeries *> ses = hisChart->series();
    if(flag)
    {
        his_data_btn->setStyleSheet("QPushButton {color:white; background:#3D6BE5; border-radius:4px}");
        foreach (QAbstractSeries *item, ses) {
            QLineSeries *tmp = (QLineSeries*)item;
            tmp->setPointsVisible(true);
        }
    }
    else
    {
        his_data_btn->setStyleSheet("QPushButton {color:black; background:#E7E7E7; border-radius:4px}");
        foreach (QAbstractSeries *item, ses) {
            QLineSeries *tmp = (QLineSeries*)item;
            tmp->setPointsVisible(false);
        }
    }
}

void UkpmWidget::showSumDataPoint(bool flag)
{
    settings->set(GPM_SETTINGS_INFO_STATS_GRAPH_POINTS,flag);

    QList<QAbstractSeries *> ses = sumChart->series();
    if(flag)
    {
        foreach (QAbstractSeries *item, ses) {
            QLineSeries *tmp = (QLineSeries*)item;
            tmp->setPointsVisible(true);
        }

    }
    else
    {
        foreach (QAbstractSeries *item, ses) {
            QLineSeries *tmp = (QLineSeries*)item;
            tmp->setPointsVisible(false);
        }
    }
}

void UkpmWidget::setDetailTab()
{
    detail_widget = new QWidget();
    tab_widget->addTab(detail_widget,tr(" detail "));

    tableView  = new QTableView();
    model = new QStandardItemModel();
    tableView->setModel(model);
    tableView->setFocusPolicy(Qt::NoFocus);
    //背景网格线设置
    //显示
    tableView->setShowGrid(false);
    //网格背景画笔
    tableView->setGridStyle(Qt::DotLine);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSelectionMode(QAbstractItemView::NoSelection);

    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    tableView->setColumnWidth(0,100);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->horizontalHeader()->hide();

    tableView->verticalHeader()->setMinimumSectionSize(0);
    tableView->verticalHeader()->setDefaultSectionSize(26);
    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    tableView->verticalHeader()->hide();

    QVBoxLayout *lay = new QVBoxLayout();
    lay->setContentsMargins(0,15,0,0);
    lay->addWidget(tableView);
    detail_widget->setLayout(lay);
}

void UkpmWidget::setHistoryTab()
{
    his_widget = new QWidget();
    QLabel *graphicType = new QLabel(tr("graphic type:"),his_widget);
    graphicType->setScaledContents(true);
    QLabel *timeLabel = new QLabel(tr("time span:"),his_widget);
    graphicType->setObjectName("m_graphicType");
    timeLabel->setObjectName("m_timeLabel");

    typeCombox = new QComboBox(his_widget);
    typeCombox->addItems(QStringList()<<tr("rate")<<tr("energy")<<tr("charge-time")<<tr("discharge-time"));
    typeCombox->setObjectName("m_typeCombox");
    spanCombox = new QComboBox(his_widget);
    spanCombox->addItems(QStringList()<<tr("ten minutes")<<tr("two hours")<<tr("six hours")<<tr("one day")<<tr("one week"));
    graphicType->setFixedHeight(36);
    timeLabel->setFixedHeight(36);
    typeCombox->setFixedHeight(36);
    spanCombox->setFixedHeight(36);

    QListView * typeView = new QListView(typeCombox);
    typeCombox->setView(typeView);
    QListView * spanView = new QListView(spanCombox);
    spanCombox->setView(spanView);

    his_line_btn = new QPushButton(tr("show line"),his_widget);
    his_data_btn = new QPushButton(tr("show points"),his_widget);
    his_line_btn->setObjectName("his_line_btn");
    his_data_btn->setObjectName("his_data_btn");
    his_line_btn->setFixedSize(100,32);
    his_data_btn->setFixedSize(100,32);
    his_line_btn->setCheckable(true);
    his_data_btn->setCheckable(true);
    his_group = new QButtonGroup;
    his_group->addButton(his_line_btn);
    his_group->addButton(his_data_btn);
    his_group->setExclusive(true);
    QVBoxLayout *checkpad = new QVBoxLayout;
    QHBoxLayout *topLayout = new QHBoxLayout;
    QHBoxLayout *bottomLayout = new QHBoxLayout;

    QFormLayout *hisType = new QFormLayout;
    hisType->addRow(graphicType,typeCombox);
    hisType->setSpacing(0);
    QFormLayout *hisSpan = new QFormLayout;
    hisSpan->addRow(timeLabel,spanCombox);
    hisSpan->setSpacing(0);
    topLayout->addLayout(hisType);
    topLayout->addLayout(hisSpan);
    topLayout->setSpacing(40);
    topLayout->setContentsMargins(0,0,0,0);

    bottomLayout->addWidget(his_line_btn,1);
    bottomLayout->addWidget(his_data_btn,1);
    bottomLayout->addLayout(checkpad,2);
    bottomLayout->setSpacing(16);

    QColor line_color("#3D6BE5");
    line_color.setAlphaF(0.8);
    hisChart = new QChart;

    hisSeries = new QLineSeries();
    hisSpline = new QScatterSeries();
    hisSpline->setMarkerSize(6);
    hisSpline->setBrush(QBrush(line_color));
    hisSpline->setBorderColor(line_color);
    QPen pen(QColor("#2ac4a1"),1);
    hisSeries->setPen(pen);

    xtime = new QCategoryAxis();//轴变量、数据系列变量，都不能声明为局部临时变量
    axisY = new QCategoryAxis();
    xtime->setTitleText(tr("elapsed time"));
    xtime->setReverse(true);
    QFont font = xtime->labelsFont();
    font.setPixelSize(12);
    xtime->setLabelsFont(font);
    axisY->setLabelsFont(font);
    font.setBold(true);
    xtime->setTitleFont(font);
    axisY->setTitleFont(font);
    xtime->setGridLinePen(QPen(QColor("#343537"),1));
    axisY->setGridLinePen(QPen(QColor("#343537"),1));
    xtime->setLineVisible(false);
    axisY->setLineVisible(false);
    hisChart->setAxisX(xtime);
    hisChart->setAxisY(axisY);

    hisChart->legend()->hide();

    hisChart->setPlotAreaBackgroundBrush(plotcolor);
    hisChart->setPlotAreaBackgroundVisible(true);
    hisChartView = new QChartView(hisChart);
    hisChartView->setRenderHint(QPainter::Antialiasing);
    hisChartView->setContentsMargins(0,0,0,0);
    hisChart->setContentsMargins(0,20,0,0);
    hisChart->setMargins(QMargins(0,0,0,0));
    QVBoxLayout *vLayout = new QVBoxLayout;

    vLayout->setSpacing(0);
    vLayout->addLayout(topLayout);
    vLayout->addWidget(hisChartView);
    vLayout->addLayout(bottomLayout);
    vLayout->setContentsMargins(0,20,0,32);
    his_widget->setLayout(vLayout);
    his_widget->hide();
    ukpm_set_choice_history();
}

void UkpmWidget::onExitButtonClicked()
{
    close();
}

void UkpmWidget::onHelpButtonClicked()
{
    ;
}

QList<QPointF> UkpmWidget::getStatics(QString stat_type)
{
    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.UPower",current_device->path,
            "org.freedesktop.UPower.Device","GetStatistics");
    QList<QPointF> list;


    msg << stat_type;
    QDBusMessage res = QDBusConnection::systemBus().call(msg);
    if(res.type() == QDBusMessage::ReplyMessage)
    {
        QDBusArgument dbusArg = res.arguments().at(0).value<QDBusArgument>();
        dbusArg >> list;
    }
    else {

    }

    return  list;
}

QList<QPointF> UkpmWidget::getHistory(QString type, uint timeSpan)
{
    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.UPower",current_device->path,
            "org.freedesktop.UPower.Device","GetHistory");
    QList<QPointF> list;
    QList<StructUdu> listQDBus;
    QVariant variant;
    uint size;
    QPointF temp;
    QDBusArgument argument;
    struct timeval tv;
    gettimeofday(&tv,NULL);

    resolution = 150;
    msg << type << timeSpan << resolution;
    QDBusMessage res = QDBusConnection::systemBus().call(msg);
    if(res.type() == QDBusMessage::ReplyMessage)
    {
        variant = res.arguments().at(0);
        argument = variant.value<QDBusArgument>();
        argument >> listQDBus;
        size = listQDBus.size();
        for(uint i = 0; i< size; i++)
        {
            if(listQDBus[i].state == 0)
                continue;
            temp.setX(tv.tv_sec - listQDBus[i].time);
            temp.setY(listQDBus[i].value);
            list.append(temp);
        }
    }
    else {
    }

    return  list;
}

QString UkpmWidget::getWidgetAxis(uint value)
{
    QString text;
    uint times = value;
    uint minutes = times/60;
    uint seconds = times - (minutes *60);
    uint hours = minutes /60;
    uint days = hours / 24;
    minutes = minutes - (hours*60);
    hours = hours - (days*24);
    if(days >0)
    {
        if(hours ==0)
            text.sprintf("%dd",days);
        else
//            text.sprintf("%dd%dh",days,hours);
            text.sprintf("%.1fd",days+hours*1.0/24);
    }
    else if(hours > 0)
    {
        if(minutes ==0)
            text.sprintf("%dh",hours);
        else
//            text.sprintf("%dh%dm",hours,minutes);
            text.sprintf("%.1fh",(hours+minutes*1.0/60));
    }
    else if(minutes > 0)
    {
        if(seconds ==0)
            text.sprintf("%dm",minutes);
        else
            text.sprintf("%dm%ds",minutes,seconds);
    }
    else
    {
        text.sprintf("%ds",seconds);
    }
    return text;

}

void UkpmWidget::onItemChanged(QListWidgetItem* cur,QListWidgetItem* pre)
{
    Q_UNUSED(pre);
    auto iterator = dev_item.find(cur);
    if(iterator != dev_item.end())
    {
        current_device = dev_item.value(cur)->m_dev;
        ukpm_update_info_data(current_device);

    }
}

void UkpmWidget::onitemSelectionChanged()
{
    QListWidgetItem *cur = listWidget->currentItem();
    int cnt = listWidget->count();
    for(int i = 0; i < cnt; i++)
    {
        QListWidgetItem *tmp = listWidget->item(i);
        auto iterator = dev_item.find(tmp);
        if(iterator != dev_item.end())
        {
            QWidget *wid = listWidget->itemWidget(tmp);
            DeviceWidget *dw = qobject_cast<DeviceWidget*>(wid);

            DEVICE *device = dev_item.value(tmp);
            QString icon = up_device_kind_to_string(device->m_dev->kind);
            if(tmp != cur)
            {
                icon = ":/resource/icon/" + icon + ".png";
                dw->set_device_icon(icon);
                dw->set_device_text(false);
            }
            else
            {
                icon = ":/resource/icon/" + icon + "w" + ".png";
                dw->set_device_text(true);
                dw->set_device_icon(icon);

                current_device = device->m_dev;
                ukpm_update_info_data(current_device);
            }
        }
    }

}

void UkpmWidget::choose_history_graph(int choice)
{
    Q_UNUSED(choice);
    QPushButton *checkedButton = qobject_cast<QPushButton*>(his_group->checkedButton());
    QString name = checkedButton->objectName();
    if(name == "his_data_btn")
    {
        his_data_btn->setStyleSheet("QPushButton {color:white; background:#3D6BE5; border-radius:4px}");
        his_line_btn->setStyleSheet("QPushButton {color:black; background:#E7E7E7; border-radius:4px}");
        hisSpline->setVisible(true);
        hisSeries->setVisible(false);
        settings->set(GPM_SETTINGS_INFO_HISTORY_GRAPH_POINTS,true);
    }
    else
    {
        his_data_btn->setStyleSheet("QPushButton {color:black; background:#E7E7E7; border-radius:4px}");
        his_line_btn->setStyleSheet("QPushButton {color:white; background:#3D6BE5; border-radius:4px}");
        hisSeries->setVisible(true);
        hisSpline->setVisible(false);
        settings->set(GPM_SETTINGS_INFO_HISTORY_GRAPH_POINTS,false);
    }
}

void UkpmWidget::choose_stat_graph(int choice)
{
    Q_UNUSED(choice);
    QPushButton *checkedButton = qobject_cast<QPushButton*>(sum_group->checkedButton());
    QString name = checkedButton->objectName();
    if(name == "sum_data_btn")
    {
        sum_data_btn->setStyleSheet("QPushButton {color:white; background:#3D6BE5; border-radius:4px}");
        sum_line_btn->setStyleSheet("QPushButton {color:black; background:#E7E7E7; border-radius:4px}");
        sumSpline->setVisible(true);
        sumSeries->setVisible(false);
        settings->set(GPM_SETTINGS_INFO_STATS_GRAPH_POINTS,true);
    }
    else
    {
        sum_data_btn->setStyleSheet("QPushButton {color:black; background:#E7E7E7; border-radius:4px}");
        sum_line_btn->setStyleSheet("QPushButton {color:white; background:#3D6BE5; border-radius:4px}");
        sumSeries->setVisible(true);
        sumSpline->setVisible(false);
        settings->set(GPM_SETTINGS_INFO_STATS_GRAPH_POINTS,false);
    }
}

void UkpmWidget::getSlots()
{

    connect(listWidget,SIGNAL(itemSelectionChanged()),this,SLOT(onitemSelectionChanged()));
    connect(tab_widget,SIGNAL(currentChanged(int)),this,SLOT(onPageChanged(int)));
    connect(typeCombox,SIGNAL(currentIndexChanged(int)),this,SLOT(upHistoryType(int)));
    connect(spanCombox,SIGNAL(currentIndexChanged(int)),this,SLOT(upHistoryType(int)));
    connect(sumTypeCombox,SIGNAL(currentIndexChanged(int)),this,SLOT(upStatsType(int)));

    connect(his_group,SIGNAL(buttonClicked(int)),this,SLOT(choose_history_graph(int)));
    connect(sum_group,SIGNAL(buttonClicked(int)),this,SLOT(choose_stat_graph(int)));

    connect(title,SIGNAL(signalButtonHelpClicked()),this,SLOT(helpFormat()));
    connect(title,SIGNAL(signalButtonCloseClicked()),this,SLOT(onExitButtonClicked()));

    for(int i = 0; i < devices.size(); i++)
    {
        QDBusConnection::systemBus().connect(DBUS_SERVICE,devices.at(i)->m_dev->path,DBUS_INTERFACE,
                                             QString("PropertiesChanged"),devices.at(i),SLOT(handleChanged(QDBusMessage)));

        connect(devices.at(i),SIGNAL(device_property_changed(QString)),this,SLOT(devPropertiesChanged(QString)));
    }
    upHistoryType(0);
    upStatsType(0);
}

void UkpmWidget::upHistoryType(int index)
{
    Q_UNUSED(index);
    int num = spanCombox->currentIndex();
    if(num == TENM)
    {
        timeSpan = 600;
    }
    else if(num == TWOH)
    {
        timeSpan = 2*60*60;
    }
    else if(num == SIXH)
    {
        timeSpan = 6*60*60;
    }
    else if(num == ONED)
    {
        timeSpan = 24*60*60;
    }
    else if(num == ONEW)
    {
        timeSpan = 7*24*60*60;
    }
    settings->set(GPM_SETTINGS_INFO_HISTORY_TIME,timeSpan);

    num = typeCombox->currentIndex();
    if (num==RATE) {

        settings->set(GPM_SETTINGS_INFO_HISTORY_TYPE,GPM_HISTORY_RATE_VALUE);

    } else if (num==VOLUME) {

        settings->set(GPM_SETTINGS_INFO_HISTORY_TYPE,GPM_HISTORY_CHARGE_VALUE);

    } else if (num==CHARGE_DURATION) {

        settings->set(GPM_SETTINGS_INFO_HISTORY_TYPE,GPM_HISTORY_TIME_FULL_VALUE);

    }else if (num==DISCHARGING_DURATION) {

        settings->set(GPM_SETTINGS_INFO_HISTORY_TYPE,GPM_HISTORY_TIME_EMPTY_VALUE);
    }
    ukpm_update_info_page_history(current_device);
}

void UkpmWidget::upStatsType(int index)
{
    Q_UNUSED(index);
    int num = sumTypeCombox->currentIndex();
    if(num == CHARGE)
    {
        settings->set(GPM_SETTINGS_INFO_STATS_TYPE,GPM_STATS_CHARGE_DATA_VALUE);
    }
    else if(num == CHARGE_ACCURENCY)
    {
        settings->set(GPM_SETTINGS_INFO_STATS_TYPE,GPM_STATS_CHARGE_ACCURACY_VALUE);
    }
    else if(num == DISCHARGING)
    {
        settings->set(GPM_SETTINGS_INFO_STATS_TYPE,GPM_STATS_DISCHARGE_DATA_VALUE);
    }
    else if(num == DISCHARGING_ACCURENCY)
    {
        settings->set(GPM_SETTINGS_INFO_STATS_TYPE,GPM_STATS_DISCHARGE_ACCURACY_VALUE);
    }
    ukpm_update_info_page_stats(current_device);
}

void UkpmWidget::onPageChanged(int index)
{
    if(index != index_old)
    {
        settings->set(GPM_SETTINGS_INFO_PAGE_NUMBER,index);
        index_old = index;
    }
    ukpm_update_info_data_page(current_device,index);
}

void UkpmWidget::drawHisSpineline(bool flag)
{
    settings->set(GPM_SETTINGS_INFO_HISTORY_GRAPH_SMOOTH,flag);
    if(flag)
    {
        his_line_btn->setStyleSheet("QPushButton {color:white; background:#3D6BE5; border-radius:4px}");
        hisChart->removeSeries(hisSeries);
        hisChart->addSeries(hisSpline);
        hisSpline->attachAxis(xtime);//连接数据集与
        hisSpline->attachAxis(axisY);//连接数据集与
        if(hisDataBox->isChecked())
            hisSpline->setPointsVisible(true);
        else
            hisSpline->setPointsVisible(false);
    }
    else
    {
        his_line_btn->setStyleSheet("QPushButton {color:black; background:#E7E7E7; border-radius:4px}");
        hisChart->removeSeries(hisSpline);
        hisChart->addSeries(hisSeries);
        hisSeries->attachAxis(xtime);//连接数据集与
        hisSeries->attachAxis(axisY);//连接数据集与
        if(hisDataBox->isChecked())
            hisSeries->setPointsVisible(true);
        else
            hisSeries->setPointsVisible(false);
    }
}

void UkpmWidget::drawSumSpineline(bool flag)
{
    settings->set(GPM_SETTINGS_INFO_STATS_GRAPH_SMOOTH,flag);
    if(flag)
    {
        sumChart->removeSeries(sumSeries);
        sumChart->addSeries(sumSpline);
        sumSpline->attachAxis(x);
        sumSpline->attachAxis(y);
        if(sumDataBox->isChecked())
            sumSpline->setPointsVisible(true);
        else
            sumSpline->setPointsVisible(false);
    }
    else
    {
        sumChart->removeSeries(sumSpline);
        sumChart->addSeries(sumSeries);
        sumSeries->attachAxis(x);
        sumSeries->attachAxis(y);
        if(sumDataBox->isChecked())
            sumSeries->setPointsVisible(true);
        else
            sumSeries->setPointsVisible(false);
    }
}

QList<QPointF> UkpmWidget::setdata() //设置图表数据的函数接口
{
    QList<QPointF> datalist;
    for (int i = 0; i < 100; i++)
        datalist.append(QPointF(i, qrand()%10));
    return datalist;
}

void UkpmWidget::control_center_power()
{
    QProcess *cmd = new QProcess(this);
    cmd->start("kylin-control-center -p");
}

void UkpmWidget::helpFormat()
{
    QProcess *cmd = new QProcess(this);
    cmd->start("yelp");
}

void UkpmWidget::devPropertiesChanged(QString object_path)
{
    if(current_device == NULL || object_path == NULL)
        return;
    if(object_path == current_device->path)
    {
        int page = tab_widget->currentIndex();
        ukpm_update_info_data_page (current_device, page);
    }
}


void UkpmWidget::deviceAdded(QDBusMessage  msg)
{
    QListWidgetItem *item;
    QDBusObjectPath objectPath;
    QString kind,vendor,model,label;
    int kindEnum = 0;
    const QDBusArgument &arg = msg.arguments().at(0).value<QDBusArgument>();
    arg >> objectPath;

    QDBusMessage msgTmp = QDBusMessage::createMethodCall(DBUS_SERVICE,objectPath.path(),
            DBUS_INTERFACE,"GetAll");
    msgTmp << DBUS_INTERFACE_PARAM;
    QDBusMessage res = QDBusConnection::systemBus().call(msgTmp);
    if(res.type() == QDBusMessage::ReplyMessage)
    {
        const QDBusArgument &dbusArg = res.arguments().at(0).value<QDBusArgument>();
        QMap<QString,QVariant> map;
        dbusArg >> map;
        kind = map.value(QString("kind")).toString();
        if(kind.length() ==0)
            kind = map.value(QString("Type")).toString();
        kindEnum = kind.toInt();
        UpDeviceKind newKind = UpDeviceKind(kindEnum);
        if(newKind != UP_DEVICE_KIND_LINE_POWER && newKind != UP_DEVICE_KIND_BATTERY && newKind != UP_DEVICE_KIND_COMPUTER)
            return;

        QString icon = up_device_kind_to_string(newKind);
        vendor = map.value(QString("Vendor")).toString();
        model = map.value(QString("Model")).toString();
        if(vendor.length() != 0 && model.length() != 0)
            label = vendor + " " + model;
        else
            label =device_kind_to_localised_text((UpDeviceKind)kindEnum,1);

        item = new QListWidgetItem(QIcon(":/"+icon),label);
        listWidget->addItem(item);
        listItem.insert(objectPath,item);

        DEVICE *dev = new DEVICE(this);
        dev->m_dev->kind = (UpDeviceKind)kindEnum;
        dev->m_dev->path = objectPath.path();
        dev->m_dev->hasHistory = map.value(QString("HasHistory")).toBool();
        dev->m_dev->hasStat = map.value(QString("HasStatistics")).toBool();

        dev->m_dev->Type = up_device_kind_to_string ((UpDeviceKind)map.value(QString("Type")).toInt());
        dev->m_dev->Model = map.value(QString("Model")).toString();
        dev->m_dev->Device = map.value(QString("NativePath")).toString();
        dev->m_dev->Vendor = map.value(QString("Vendor")).toString();
        dev->m_dev->Capacity = QString::number(map.value(QString("Capacity")).toDouble(), 'f', 1) + "%";
        dev->m_dev->Energy = QString::number(map.value(QString("Energy")).toDouble(), 'f', 1)+ " Wh";
        dev->m_dev->EnergyEmpty= QString::number(map.value(QString("EnergyEmpty")).toDouble(), 'f', 1)+ " Wh";
        dev->m_dev->EnergyFull = QString::number(map.value(QString("EnergyFull")).toDouble(), 'f', 1)+ " Wh";
        dev->m_dev->EnergyFullDesign = QString::number(map.value(QString("EnergyFullDesign")).toDouble(), 'f', 1) + " Wh";
        dev->m_dev->EnergyRate = QString::number(map.value(QString("EnergyRate")).toDouble(), 'f', 1) + " W";
        dev->m_dev->Percentage = QString::number(map.value(QString("Percentage")).toDouble(), 'f', 1)+"%";
        dev->m_dev->Online = boolToString(map.value(QString("Online")).toBool());
        dev->m_dev->IsPresent = boolToString(map.value(QString("IsPresent")).toBool());
        dev->m_dev->IsRechargeable = boolToString(map.value(QString("IsRechargeable")).toBool());
        dev->m_dev->PowerSupply = boolToString(map.value(QString("PowerSupply")).toBool());

        int flag = map.value(QString("State")).toLongLong();
        switch (flag) {
        case 1:
            dev->m_dev->State = tr("Charging");
            break;
        case 2:
            dev->m_dev->State = tr("Discharging");
            break;
        case 3:
            dev->m_dev->State = tr("Empty");
            break;
        case 4:
            dev->m_dev->State = tr("Charged");
            break;
        case 5:
            dev->m_dev->State = tr("PendingCharge");
            break;
        case 6:
            dev->m_dev->State = tr("PendingDischarge");
            break;
        default:
            break;
        }
        calcTime(dev->m_dev->TimeToEmpty, map.value(QString("TimeToEmpty")).toLongLong());
        calcTime(dev->m_dev->TimeToFull, map.value(QString("TimeToFull")).toLongLong());
        dev->m_dev->Voltage = QString::number(map.value(QString("Voltage")).toDouble(), 'f', 1) + " V";
        devices.push_back(dev);

        QDBusConnection::systemBus().connect(DBUS_SERVICE,dev->m_dev->path,DBUS_INTERFACE,
                                             QString("PropertiesChanged"),dev,SLOT(handleChanged(QDBusMessage)));

        connect(dev,SIGNAL(device_property_changed(QString)),this,SLOT(devPropertiesChanged(QString)));
    }
}

void UkpmWidget::deviceRemoved(QDBusMessage  msg)
{
    const QDBusArgument &arg = msg.arguments().at(0).value<QDBusArgument>();
    QDBusObjectPath objectPath;
    arg >> objectPath;
    QMap<QDBusObjectPath,QListWidgetItem*>::iterator iter = listItem.find(objectPath);
    if(iter!= listItem.end())
    {
        if(dev_item.contains(iter.value()))
        {
            auto dev_item_iter = dev_item.find(iter.value());
            dev_item.erase(dev_item_iter);
        }
        listWidget->removeItemWidget(iter.value());
        listItem.erase(iter);
        delete iter.value();
    }
    foreach (auto item, devices) {
        if(item->m_dev->path == objectPath.path())
        {
            devices.removeOne(item);
            delete item;
            break;
        }
    }

}


void UkpmWidget::setupUI()
{
    QDesktopWidget *deskdop = QApplication::desktop();
    setFixedSize(860,580);
    move((deskdop->width() - this->width())/2, (deskdop->height() - this->height())/2);

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    QFrame *frame = new QFrame(this);
    frame->setObjectName("main_frame");
    frame->setStyleSheet("QFrame#main_frame{background-color: white;border-radius:6px;border:1px solid #c2c3c8;}"); //设置圆角与背景透明

    QSplitter *mainsplitter = new QSplitter(Qt::Horizontal);//splittering into two parts
    QHBoxLayout *deviceLay = new QHBoxLayout;
    deviceLay->setContentsMargins(0,0,40,0);
    deviceLay->setSpacing(40);
    listWidget = new QListWidget(mainsplitter);
    listWidget->setObjectName("m_listWidget");
    listWidget->setFixedWidth(180);
    listWidget->setSpacing(8);
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    deviceLay->addWidget(listWidget);
    tab_widget =  new QTabWidget(this);
    tab_widget->setStyle(QStyleFactory::create("fusion"));
    deviceLay->addWidget(tab_widget);

    QVBoxLayout *vlayout = new QVBoxLayout;
    QFrame *header = new QFrame(frame);
    header->setFixedHeight(TITLE_HEIGHT+30);

    QLabel *header_head = new QLabel(header);
    header_head->setGeometry(0,0,180,TITLE_HEIGHT+30);
    header_head->setStyleSheet("background-color:#F7F7F7;");
    header->setWindowFlags(Qt::FramelessWindowHint);
    header->setStyleSheet("QFrame{padding-bottom:30px;background-color:white;border-top-right-radius:6px;"
                          "border-top-left-radius:6px;border-bottom-right-radius:0px;border-bottom-left-radius:0px;}");

    vlayout->addWidget(header);
    vlayout->addLayout(deviceLay);
    vlayout->setSpacing(0);
    vlayout->setContentsMargins(0,0,0,0);
    frame->setLayout(vlayout);//main layout of the UI
    QVBoxLayout *mainlay = new QVBoxLayout;
    mainlay->setContentsMargins(0,0,0,0);
    this->setLayout(mainlay);
    mainlay->addWidget(frame);
    title = new TitleWidget(this);
    title->move(0,0);

    setDetailTab();
    setHistoryTab();
    setSumTab();
    getDevices();

    current_device = NULL;
    if(devices.size()>0)
    {
        current_device = devices.at(0)->m_dev;
        QListWidgetItem *default_item = listWidget->item(0);
        listWidget->setItemSelected(default_item,true);
        listWidget->setCurrentItem(default_item);

        QString icon = up_device_kind_to_string(current_device->kind);
        icon = ":/resource/icon/" + icon + "w" + ".png";
        QWidget *wid = listWidget->itemWidget(default_item);
        DeviceWidget *dw = qobject_cast<DeviceWidget *>(wid);
        dw->set_device_icon(icon);
        dw->set_device_text(true);
        ukpm_update_info_data (current_device);
    }

}

void UkpmWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setPen(Qt::NoPen);
    QPainterPath path;
    path.addRoundedRect(rect(),6,6);
    p.setRenderHint(QPainter::Antialiasing);
    setProperty("blurRegion",QRegion(path.toFillPolygon().toPolygon()));
    QWidget::paintEvent(event);
}
