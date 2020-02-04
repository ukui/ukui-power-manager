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
#include "ukpm-widget.h"
#include "customtype.h"
#include "sys/time.h"

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
    plotcolor.setNamedColor("#F1F1F1");
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
    qDebug()<< "arguments size is:"<<arguments.size();
    if(arguments.size() < 3)
        return index;
    QString dev = arguments.at(2);
    int size = deviceNames.count();
    for(int i = 0; i< size; i++)
        if(deviceNames.at(i).path() == dev)
            index = i;
    qDebug()<< dev << " of index=" << index;
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
        qDebug()<<"get devices size!"<<deviceNames.size();
    }
    else {
        qDebug()<<"No response!";
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
                item = new QListWidgetItem(QIcon(":/images/"+icon+".png"),label);
                listItem.insert(deviceNames.at(i),item);
//                listWidget->insertItem(i,item);
                listWidget->addItem(item);
            }
            else
                return;

            DEVICE *dev = new DEVICE(this);
            dev->m_dev->kind = (UpDeviceKind)kindEnum;
            dev->m_dev->path = deviceNames.at(i).path();
            qDebug()<<dev->m_dev->path;
            dev->m_dev->hasHistory = map.value(QString("HasHistory")).toBool();
            dev->m_dev->hasStat = map.value(QString("HasStatistics")).toBool();
//            dev->Type = up_device_kind_to_string ((UpDeviceKind)map.value(QString("Type")).toInt());
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
//            struct timeval tv;
//            uint tim;
//            gettimeofday(&tv,NULL);
//            tim = tv.tv_sec - map.value(QString("UpdateTime")).toLongLong();
//            dev->m_dev.Refresh = getSufix(tim,'s');

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
//            dev_item.insert(dev,item);
            dev_item.insert(item,dev);
        }
    }
//    if(!batterySvr.isEmpty())
//    {
//        memcpy(&btrDetailData,&devices.at(0),sizeof(DEV));
//    	setupBtrUI();
//    }
//    if(!acSvr.isEmpty())
//    {
//        memcpy(&dcDetailData,&devices.at(1),sizeof(DEV));
//    	setupDcUI();
//    }

//    setupBtrUI();
//    setupDcUI();

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
    model->setHorizontalHeaderLabels(header);

    addListRow(tr("Device"),device_path);
    /* TRANSLATORS: the device ID of the current device, e.g. "battery0" */
    addListRow(tr("Type"),dev.Type);

    if (dev.Vendor != NULL && dev.Vendor[0] != '\0')
        addListRow (tr("Vendor"), dev.Vendor);
    if (dev.Model != NULL && dev.Model != '\0')
        addListRow (tr("Model"), dev.Model);
    if (dev.serial != NULL && dev.serial[0] != '\0')
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

    if (has_history)
        tab_widget->addTab(his_widget,tr("history"));
    else
    {
        index = tab_widget->indexOf(his_widget);
        tab_widget->removeTab(index);
    }
    if (has_statistics)
        tab_widget->addTab(stat_widget,tr("statistics"));
    else
    {
        index = tab_widget->indexOf(stat_widget);
        tab_widget->removeTab(index);
    }

    page = tab_widget->currentIndex();
    index_old = page;
//    qDebug()<<"ukpm_update_info_data: page="<<page;
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
        current_device->autorange_x = FALSE;
        current_device->start_x = -timeSpan;
        current_device->stop_x = 0;
        current_device->autorange_y = FALSE;
        current_device->start_y = 0;
        current_device->stop_y = 100;
        type = "rate";

    } else if (index==VOLUME) {

        current_device->type_x = GPM_GRAPH_WIDGET_TYPE_TIME;
        current_device->type_y = GPM_GRAPH_WIDGET_TYPE_POWER;
        current_device->autorange_x = FALSE;
        current_device->start_x = -timeSpan;
        current_device->stop_x = 0;
        current_device->autorange_y = true;
        type = "charge";

    } else if (index==CHARGE_DURATION) {

        current_device->type_x = GPM_GRAPH_WIDGET_TYPE_TIME;
        current_device->type_y = GPM_GRAPH_WIDGET_TYPE_TIME;
        current_device->autorange_x = FALSE;
        current_device->start_x = -timeSpan;
        current_device->stop_x = 0;
        current_device->autorange_y = true;
        type = "time-full";

    }else if (index==DISCHARGING_DURATION) {

        current_device->type_x = GPM_GRAPH_WIDGET_TYPE_TIME;
        current_device->type_y = GPM_GRAPH_WIDGET_TYPE_TIME;
        current_device->autorange_x = FALSE;
        current_device->start_x = -timeSpan;
        current_device->stop_x = 0;
        current_device->autorange_y = true;
        type = "time-empty";
    }
    QList<QPointF> list = getHistory(type,timeSpan);
    if (list.size() == 0) {
        return;
    }
//    ukpm_set_choice_history();
    draw_history_graph(list);
}

void UkpmWidget::draw_stats_graph(QList<QPointF> list)
{
    int start_x = 0;
    float max_y = 0;
    QPointF pit;
    QList<QPointF> data;
    int index = sumTypeCombox->currentIndex();
    if(index == CHARGE)
    {
        foreach(pit, list)
        {
            data.append(QPointF(start_x++,pit.x()));
            if(max_y < fabs(pit.x()))
                max_y = fabs(pit.x());
        }
        max_y = ceil(max_y);
        sumSeries->replace(data);
        sumSpline->replace(data);
        y->setTitleText(tr("adjust factor"));
        y->setRange(-max_y,+max_y);
        y->setLabelFormat("%.1f");
        x->setTitleText(tr("battery power"));
        x->setRange(0,100);
        x->setLabelFormat("%d%");
        x->setTickCount(10);
        y->setTickCount(10);

    }
    else if(index == CHARGE_ACCURENCY)
    {
        foreach(pit, list)
        {
            data.append(QPointF(start_x++,pit.y()));
            if(max_y < (pit.y()))
                max_y = (pit.y());
        }
        sumSeries->replace(data);
        sumSpline->replace(data);
        y->setTitleText(tr("Predict Accurency"));
        y->setRange(0,max_y);
        y->setLabelFormat("%d%");
        x->setTitleText(tr("battery power"));
        x->setRange(0,100);
        x->setLabelFormat("%d%");
        x->setTickCount(10);
        y->setTickCount(10);
    }
    else if(index == DISCHARGING)
    {
        foreach(pit, list)
        {
            data.append(QPointF(start_x++,pit.x()));
            if(max_y < fabs(pit.x()))
                max_y = fabs(pit.x());
        }
        max_y = ceil(max_y);
        sumSeries->replace(data);
        sumSpline->replace(data);
        y->setTitleText(tr("adjust factor"));
        y->setRange(-max_y,+max_y);
        y->setLabelFormat("%.1f");
        x->setTitleText(tr("battery power"));
        x->setRange(0,100);
        x->setLabelFormat("%d%");
        x->setTickCount(10);
        y->setTickCount(10);
    }
    else if(index == DISCHARGING_ACCURENCY)
    {
        foreach(pit, list)
        {
            data.append(QPointF(start_x++,pit.y()));
            if(max_y < (pit.y()))
                max_y = (pit.y());
        }
        sumSeries->replace(data);
        sumSpline->replace(data);
        y->setTitleText(tr("Predict Accurency"));
        y->setRange(0,max_y);
        y->setLabelFormat("%d%");
        x->setTitleText(tr("battery power"));
        x->setRange(0,100);
        x->setLabelFormat("%d%");
        x->setTickCount(10);
        y->setTickCount(10);
    }

    sumStack->setCurrentIndex(1);
}

void UkpmWidget::ukpm_set_choice_sum()
{
    bool checked;

    QString stats_type = settings->getString(GPM_SETTINGS_INFO_STATS_TYPE);
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

//    Q_EMIT hisDataBox->clicked(checked);
    checked = settings->getBool(GPM_SETTINGS_INFO_STATS_GRAPH_SMOOTH);
    sumCurveBox->setChecked(checked);
//    Q_EMIT sumCurveBox->clicked(checked);
    checked = settings->getBool(GPM_SETTINGS_INFO_STATS_GRAPH_POINTS);
    sumDataBox->setChecked(checked);
//    Q_EMIT sumDataBox->clicked(checked);
    if(sumDataBox->isChecked())
    {
        if(sumCurveBox->isChecked())
        {
            sumSpline->setPointsVisible(true);
            sumChart->removeSeries(sumSeries);
            sumChart->addSeries(sumSpline);
            sumSpline->attachAxis(x);//连接数据集与
            sumSpline->attachAxis(y);//连接数据集与
        }
        else
        {
            sumSeries->setPointsVisible(true);
            sumChart->removeSeries(sumSpline);
            sumChart->addSeries(sumSeries);
            sumSeries->attachAxis(x);//连接数据集与
            sumSeries->attachAxis(y);//连接数据集与
        }
    }
    else
    {
        if(sumCurveBox->isChecked())
        {
            sumSpline->setPointsVisible(false);
            sumChart->removeSeries(sumSeries);
            sumChart->addSeries(sumSpline);
            sumSpline->attachAxis(x);//连接数据集与
            sumSpline->attachAxis(y);//连接数据集与
        }
        else
        {
            sumSeries->setPointsVisible(false);
            sumChart->removeSeries(sumSpline);
            sumChart->addSeries(sumSeries);
            sumSeries->attachAxis(x);//连接数据集与
            sumSeries->attachAxis(y);//连接数据集与
        }
    }
}

void UkpmWidget::draw_history_graph(QList<QPointF> list)
{
    QStringList labels;
    uint max_y = 0;
    uint min_y = 0;
    int index = typeCombox->currentIndex();
    //y
    if (index==RATE) {
        axisY->setTitleText(tr("Rate"));
         labels = axisY->categoriesLabels();
        foreach (QString str, labels) {
            axisY->remove(str);
        }
        axisY->setMin(0.0);
        axisY->setMax(10.0);
        axisY->setStartValue(0.0);
        for(int i = 0; i < 11; i++)
        {
            QString str;
            str.sprintf("%.1fW",i*1.0);
            axisY->append(str,i*1.0);
        }
        axisY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);

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
        min_y = max_y = list.at(0).y();
        for(int i = 0; i< size; i++)
        {
            uint tmp = list.at(i).y();
            if(min_y > tmp)
                min_y = tmp;
            if(max_y < tmp)
                max_y = tmp;
        }

        min_y = floor(min_y/600.0) * 600;
        max_y = ceil(max_y/600.0) * 600;

        axisY->setMin(min_y);
        axisY->setMax(max_y);
        axisY->setStartValue(min_y);
        for(int i = 0; i < 11; i++)
        {
            axisY->append(getWidgetAxis(min_y+i*(max_y-min_y)/10),min_y+i*(max_y-min_y)/10);
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
        min_y = max_y = list.at(0).y();
        for(int i = 0; i< size; i++)
        {
            uint tmp = list.at(i).y();
            if(min_y > tmp)
                min_y = tmp;
            if(max_y < tmp)
                max_y = tmp;
        }

        min_y = floor(min_y/600.0) * 600;
        max_y = ceil(max_y/600.0) * 600;

        axisY->setMin(min_y);
        axisY->setMax(max_y);
        axisY->setStartValue(min_y);

        for(int i = 0; i < 11; i++)
        {
            axisY->append(getWidgetAxis(min_y+i*(max_y-min_y)/10),min_y+i*(max_y-min_y)/10);
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
    hisStack->setCurrentIndex(1);

}

void UkpmWidget::ukpm_set_choice_history()
{
    bool checked;

    QString history_type = settings->getString(GPM_SETTINGS_INFO_HISTORY_TYPE);
    int history_time = settings->getInt(GPM_SETTINGS_INFO_HISTORY_TIME);
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

//    Q_EMIT spanCombox->currentIndexChanged(spanCombox->currentIndex());
//    Q_EMIT sumTypeCombox->currentIndexChanged(sumTypeCombox->currentIndex());
    checked = settings->getBool(GPM_SETTINGS_INFO_HISTORY_GRAPH_SMOOTH);
    hisCurveBox->setChecked(checked);
//    Q_EMIT hisCurveBox->clicked(checked);
    checked = settings->getBool(GPM_SETTINGS_INFO_HISTORY_GRAPH_POINTS);
    hisDataBox->setChecked(checked);
//    Q_EMIT hisDataBox->clicked(checked);

    if(hisDataBox->isChecked())
    {
        if(hisCurveBox->isChecked())
        {
            hisSpline->setPointsVisible(true);
            hisChart->removeSeries(hisSeries);
            hisChart->addSeries(hisSpline);
            hisSpline->attachAxis(xtime);//连接数据集与
            hisSpline->attachAxis(axisY);//连接数据集与
        }
        else
        {
            hisSeries->setPointsVisible(true);
            hisChart->removeSeries(hisSpline);
            hisChart->addSeries(hisSeries);
            hisSeries->attachAxis(xtime);//连接数据集与
            hisSeries->attachAxis(axisY);//连接数据集与
        }
    }
    else
    {
        if(hisCurveBox->isChecked())
        {
            hisSpline->setPointsVisible(false);
            hisChart->removeSeries(hisSeries);
            hisChart->addSeries(hisSpline);
            hisSpline->attachAxis(xtime);//连接数据集与
            hisSpline->attachAxis(axisY);//连接数据集与
        }
        else
        {
            hisSeries->setPointsVisible(false);
            hisChart->removeSeries(hisSpline);
            hisChart->addSeries(hisSeries);
            hisSeries->attachAxis(xtime);//连接数据集与
            hisSeries->attachAxis(axisY);//连接数据集与
        }
    }
}
void UkpmWidget::ukpm_update_info_data_page (DEV* device, gint page)
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
        settings->setString(GPM_SETTINGS_INFO_STATS_TYPE,GPM_STATS_CHARGE_DATA_VALUE);
    }
    else if(index == CHARGE_ACCURENCY)
    {
        type = "charging";
        settings->setString(GPM_SETTINGS_INFO_STATS_TYPE,GPM_STATS_CHARGE_ACCURACY_VALUE);
    }
    else if(index == DISCHARGING)
    {
        type = "discharging";
        settings->setString(GPM_SETTINGS_INFO_STATS_TYPE,GPM_STATS_DISCHARGE_DATA_VALUE);
    }
    else if(index == DISCHARGING_ACCURENCY)
    {
        type = "discharging";
        settings->setString(GPM_SETTINGS_INFO_STATS_TYPE,GPM_STATS_DISCHARGE_ACCURACY_VALUE);
    }

    QList<QPointF> list = getStatics(type);
    if (list.size() == 0) {
        return;
    }
    draw_stats_graph (list);
//    ukpm_set_choice_sum();

}

void UkpmWidget::setSumTab()
{
    stat_widget = new QWidget(this);
    QLabel *graphicType = new QLabel(tr("graphic type:"),stat_widget);
    graphicType->setObjectName("m_sumGraphicType");
    sumTypeCombox = new QComboBox(stat_widget);
    sumTypeCombox->addItems(QStringList()<<tr("charge")<<tr("charge-accurency")<<tr("discharge")<<tr("discharge-accurency"));

    QListView * listView = new QListView(sumTypeCombox);
    listView->setStyleSheet("QListView::item:selected {background: #EDEDED }");
    sumTypeCombox->setView(listView);
    sumCurveBox = new QCheckBox(tr("spline"),stat_widget);
    sumDataBox = new QCheckBox(tr("show datapoint"),stat_widget);
    sumCurveBox->setFixedSize(110,24);
    sumDataBox->setFixedSize(110,24);
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    QFormLayout *topFormLayout = new QFormLayout;

    graphicType->setFixedWidth(80);
    sumTypeCombox->setFixedWidth(200);
    topFormLayout->addRow(graphicType,sumTypeCombox);
    topFormLayout->setSpacing(0);

    sumChart = new QChart;

    x = new QValueAxis;
    y = new QValueAxis;

    sumSeries = new QLineSeries();
    sumSpline = new QSplineSeries();
    sumChart->setAxisX(x);
    sumChart->setAxisY(y);

    QFont font("Times");
    font.setPixelSize(12);
    x->setLabelsFont(font);
    y->setLabelsFont(font);
    font.setBold(true);
    x->setTitleFont(font);
    y->setTitleFont(font);

    sumChart->legend()->hide();

    sumChart->setPlotAreaBackgroundBrush(plotcolor);
    sumChart->setPlotAreaBackgroundVisible(true);

    sumChartView = new QChartView(sumChart);
    sumChartView->setRenderHint(QPainter::Antialiasing);
    sumChartView->setFixedWidth(594);
    QVBoxLayout *botchild = new QVBoxLayout;
    bottomLayout->addWidget(sumCurveBox,1);
    bottomLayout->addWidget(sumDataBox,1);
    bottomLayout->addLayout(botchild,4);
    bottomLayout->setSpacing(40);

    QVBoxLayout *vLayout = new QVBoxLayout;

    sumStack = new QStackedWidget;
    QLabel *nodata = new QLabel;
    nodata->setText(tr("no data to show."));
    nodata->setAlignment(Qt::AlignCenter);
    sumStack->addWidget(nodata);
    sumStack->addWidget(sumChartView);

    vLayout->addLayout(topFormLayout);
    vLayout->addWidget(sumStack);
    vLayout->addLayout(bottomLayout);
    stat_widget->setLayout(vLayout);
    stat_widget->hide();
    ukpm_set_choice_sum();
}
void UkpmWidget::showHisDataPoint(bool flag)
{
    settings->setBool(GPM_SETTINGS_INFO_HISTORY_GRAPH_POINTS,flag);
    QList<QAbstractSeries *> ses = hisChart->series();
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

void UkpmWidget::showSumDataPoint(bool flag)
{
    settings->setBool(GPM_SETTINGS_INFO_STATS_GRAPH_POINTS,flag);

    QList<QAbstractSeries *> ses = sumChart->series();
    if(flag)
    {
//        qDebug()<<"showSumDataPoint---true";
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
    detail_widget = new QWidget(this);
    tab_widget->addTab(detail_widget,tr("detail"));

    tableView  = new QTableView();
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    tableView->verticalHeader()->hide();
    //背景网格线设置
    //显示
    tableView->setShowGrid(false);
    //网格背景画笔
    //ui->tableView->setGridStyle(Qt::DashLine);
    tableView->setGridStyle(Qt::DotLine);
    //排序功能
    tableView->setSortingEnabled(true);
    model = new QStandardItemModel();

    tableView->setModel(model);
    QVBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(tableView);
    detail_widget->setLayout(lay);

}

void UkpmWidget::setHistoryTab()
{
    his_widget = new QWidget(this);
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
    graphicType->setFixedWidth(80);
    typeCombox->setFixedWidth(200);
    timeLabel->setFixedWidth(80);
    spanCombox->setFixedWidth(200);

    QListView * typeView = new QListView(typeCombox);
    typeView->setStyleSheet("QListView::item:selected {background: #EDEDED }");
    typeCombox->setView(typeView);
    QListView * spanView = new QListView(spanCombox);
    spanView->setStyleSheet("QListView::item:selected {background: #EDEDED }");
    spanCombox->setView(spanView);

    hisCurveBox = new QCheckBox(tr("spline"),his_widget);
    hisDataBox = new QCheckBox(tr("show datapoint"),his_widget);
    hisCurveBox->setFixedSize(110,24);
    hisDataBox->setFixedSize(110,24);
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

    bottomLayout->addWidget(hisCurveBox,1);
    bottomLayout->addWidget(hisDataBox,1);
    bottomLayout->addLayout(checkpad,2);
    bottomLayout->setSpacing(40);
//    bottomLayout->setAlignment(hisCurveBox,Qt::AlignLeft);
//    bottomLayout->setAlignment(hisDataBox,Qt::AlignLeft);

    hisChart = new QChart;

    hisSeries = new QLineSeries();
    hisSpline = new QSplineSeries();
//    hisChart->addSeries(hisSpline);

    xtime = new QCategoryAxis();//轴变量、数据系列变量，都不能声明为局部临时变量
    axisY = new QCategoryAxis();
    xtime->setTitleText(tr("elapsed time"));
    xtime->setReverse(true);
    QFont font("Times");
    font.setPixelSize(12);
    xtime->setLabelsFont(font);
    axisY->setLabelsFont(font);
    font.setBold(true);
    xtime->setTitleFont(font);
    axisY->setTitleFont(font);
    hisChart->setAxisX(xtime);
    hisChart->setAxisY(axisY);

    hisChart->legend()->hide();

    hisChart->setPlotAreaBackgroundBrush(plotcolor);
    hisChart->setPlotAreaBackgroundVisible(true);
    hisChartView = new QChartView(hisChart);
    hisChartView->setRenderHint(QPainter::Antialiasing);
    hisChartView->setFixedWidth(594);
    QVBoxLayout *vLayout = new QVBoxLayout;
    hisStack = new QStackedWidget;
    QLabel *nodata = new QLabel;
    nodata->setText(tr("no data to show."));
    nodata->setAlignment(Qt::AlignCenter);
    hisStack->addWidget(nodata);
    hisStack->addWidget(hisChartView);
    vLayout->addLayout(topLayout);
    vLayout->addWidget(hisStack);
    vLayout->addLayout(bottomLayout);
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
//    QString sumType;
//    QList<QPointF> data;
//    QPointF pit;
//    int start_x = 0;
//    float max_y = 0;
//    sumType = GPM_STATS_CHARGE_DATA_VALUE;
//    settings->setString(GPM_SETTINGS_INFO_STATS_TYPE,sumType);

    msg << stat_type;
    QDBusMessage res = QDBusConnection::systemBus().call(msg);
    if(res.type() == QDBusMessage::ReplyMessage)
    {
        printf("get %d arg from bus!\n",res.arguments().count());
        QDBusArgument dbusArg = res.arguments().at(0).value<QDBusArgument>();
        dbusArg >> list;
    }
    else {
        qDebug()<<"error of qdbus reply";

    }
    if(list.isEmpty())
    {
        sumStack->setCurrentIndex(0);
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
//    uint max_y = 0;
//    uint min_y = 0;
//    bool first_run = true;
    resolution = 150;
//    settings->setString(GPM_SETTINGS_INFO_HISTORY_TYPE,type);
    msg << type << timeSpan << resolution;
    QDBusMessage res = QDBusConnection::systemBus().call(msg);
    if(res.type() == QDBusMessage::ReplyMessage)
    {
        variant = res.arguments().at(0);
        argument = variant.value<QDBusArgument>();
        argument >> listQDBus;
        size = listQDBus.size();
//        qDebug()<<"size="<<size<<"span="<<timeSpan;
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
        qDebug()<<"gethistory:error of qdbus reply";
    }
    if(list.isEmpty())
    {
        hisStack->setCurrentIndex(0);
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
            text.sprintf("%dd%dh",days,hours);
    }
    else if(hours > 0)
    {
        if(minutes ==0)
            text.sprintf("%dh",hours);
        else
            text.sprintf("%dh%dm",hours,minutes);
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
        current_device = dev_item.value(cur)->m_dev;
//    qDebug()<<"onItemChanged";
    ukpm_update_info_data(current_device);
}

void UkpmWidget::getSlots()
{

    QDBusConnection::systemBus().connect(DBUS_SERVICE,DBUS_OBJECT,DBUS_SERVICE,
                                         QString("device-added"),this,SLOT(deviceAdded(QDBusMessage)));
    QDBusConnection::systemBus().connect(DBUS_SERVICE,DBUS_OBJECT,DBUS_SERVICE,
                                         QString("device-removed"),this,SLOT(deviceRemoved(QDBusMessage)));

    connect(listWidget,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(onItemChanged(QListWidgetItem*,QListWidgetItem*)));
    connect(tab_widget,SIGNAL(currentChanged(int)),this,SLOT(onPageChanged(int)));
    connect(typeCombox,SIGNAL(currentIndexChanged(int)),this,SLOT(upHistoryType(int)));
    connect(spanCombox,SIGNAL(currentIndexChanged(int)),this,SLOT(upHistoryType(int)));
    connect(sumTypeCombox,SIGNAL(currentIndexChanged(int)),this,SLOT(upStatsType(int)));

    connect(sumDataBox,SIGNAL(toggled(bool)),this,SLOT(showSumDataPoint(bool)));
    connect(hisDataBox,SIGNAL(clicked(bool)),this,SLOT(showHisDataPoint(bool)));
    connect(hisCurveBox,SIGNAL(toggled(bool)),this,SLOT(drawHisSpineline(bool)));
    connect(sumCurveBox,SIGNAL(toggled(bool)),this,SLOT(drawSumSpineline(bool)));
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
//    qDebug()<<"upHistoryType";
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
    settings->setInt(GPM_SETTINGS_INFO_HISTORY_TIME,timeSpan);

    num = typeCombox->currentIndex();
    if (num==RATE) {

        settings->setString(GPM_SETTINGS_INFO_HISTORY_TYPE,GPM_HISTORY_RATE_VALUE);

    } else if (num==VOLUME) {

        settings->setString(GPM_SETTINGS_INFO_HISTORY_TYPE,GPM_HISTORY_CHARGE_VALUE);

    } else if (num==CHARGE_DURATION) {

        settings->setString(GPM_SETTINGS_INFO_HISTORY_TYPE,GPM_HISTORY_TIME_FULL_VALUE);

    }else if (num==DISCHARGING_DURATION) {

        settings->setString(GPM_SETTINGS_INFO_HISTORY_TYPE,GPM_HISTORY_TIME_EMPTY_VALUE);
    }
    ukpm_update_info_page_history(current_device);
}

void UkpmWidget::upStatsType(int index)
{
    Q_UNUSED(index);
//    qDebug()<<"upStatsType";
    int num = sumTypeCombox->currentIndex();
    if(num == CHARGE)
    {
        settings->setString(GPM_SETTINGS_INFO_STATS_TYPE,GPM_STATS_CHARGE_DATA_VALUE);
    }
    else if(num == CHARGE_ACCURENCY)
    {
        settings->setString(GPM_SETTINGS_INFO_STATS_TYPE,GPM_STATS_CHARGE_ACCURACY_VALUE);
    }
    else if(num == DISCHARGING)
    {
        settings->setString(GPM_SETTINGS_INFO_STATS_TYPE,GPM_STATS_DISCHARGE_DATA_VALUE);
    }
    else if(num == DISCHARGING_ACCURENCY)
    {
        settings->setString(GPM_SETTINGS_INFO_STATS_TYPE,GPM_STATS_DISCHARGE_ACCURACY_VALUE);
    }
    ukpm_update_info_page_stats(current_device);
}

void UkpmWidget::onPageChanged(int index)
{
    if(index != index_old)
    {
        settings->setInt(GPM_SETTINGS_INFO_PAGE_NUMBER,index);
        index_old = index;
    }
//    qDebug()<<"tab page number is:"<< index;
//    ukpm_update_info_data_page(current_device,index);
}

void UkpmWidget::drawHisSpineline(bool flag)
{
    settings->setBool(GPM_SETTINGS_INFO_HISTORY_GRAPH_SMOOTH,flag);
//    ukpm_update_info_page_history(current_device);
    if(flag)
    {
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
    settings->setBool(GPM_SETTINGS_INFO_STATS_GRAPH_SMOOTH,flag);
//    ukpm_update_info_page_stats(current_device);
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
//        qDebug()<<"devproperty changed and page="<<page;
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
        qDebug()<<dev->m_dev->path;
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
//    QMap<QDBusObjectPath,QTabWidget*>::iterator iterWidget = widgetItem.find(objectPath);
//    if(iterWidget!= widgetItem.end())
//    {
//        stackedWidget->removeWidget(iterWidget.value());
//        widgetItem.erase(iterWidget);
//        delete iterWidget.value();
//    }
}


void UkpmWidget::setupUI()
{
    QDesktopWidget *deskdop = QApplication::desktop();
//    resize(deskdop->width()/2,deskdop->height()/2);
    resize(900,580);
    move((deskdop->width() - this->width())/2, (deskdop->height() - this->height())/2);
//    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);
//    setWindowTitle(tr("Power Statistics"));
    setWindowFlags(Qt::FramelessWindowHint);
//    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowMinimizeButtonHint);
    QSplitter *mainsplitter = new QSplitter(Qt::Horizontal,this);//splittering into two parts
    listWidget = new QListWidget(mainsplitter);
    listWidget->setObjectName("m_listWidget");
    listWidget->setSpacing(10);
    tab_widget =  new QTabWidget(mainsplitter);

    QList<int> list_src;
    list_src.append(180);
    list_src.append(680);
    mainsplitter->setSizes(list_src);
//    mainsplitter->setStretchFactor(1,4);
//    mainsplitter->setFrameStyle();
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setContentsMargins(5,0,40,0);
    QFrame *header = new QFrame(this);
    header->setFixedHeight(TITLE_HEIGHT);
    header->setWindowFlags(Qt::FramelessWindowHint);
    vlayout->addWidget(header);
    vlayout->addWidget(mainsplitter);

    setLayout(vlayout);//main layout of the UI
    title = new TitleWidget(this);
    title->move(0,0);

    setDetailTab();
    setHistoryTab();
    setSumTab();
    getDevices();

    if(devices.size()>0)
    {
        current_device = devices.at(0)->m_dev;

        listWidget->setItemSelected(listWidget->item(0),true);
        ukpm_update_info_data (current_device);
    }
    else {
        current_device = NULL;
    }
}

