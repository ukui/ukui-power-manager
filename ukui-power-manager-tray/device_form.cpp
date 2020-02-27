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
#include "device_form.h"
#include "ui_deviceform.h"
#include <QIcon>
#include <QDebug>
#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>

DeviceForm::DeviceForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceForm)
{
    ui->setupUi(this);
    mov = new QMovie(":/22x22/charging.gif");
    setAttribute(Qt::WA_StyledBackground,true);
    ed = EngineDevice::getInstance();
//    connect(ed,SLOT(signal_device_change(DEVICE*)),this,SLOT(slot_device_change(DEVICE*)));
}

DeviceForm::~DeviceForm()
{
    delete mov;
    delete ui;
}

void DeviceForm::setIcon(QString name)
{
    if(name.contains("charging"))
    {
        ui->icon->setMovie(mov);
        mov->start();
        return;
    }
    mov->stop();
    QIcon icon = QIcon::fromTheme(name);
    qDebug()<<icon.name()<<"-----------this is device icon---------------------";
    QPixmap pixmap = icon.pixmap(QSize(32,32));
    ui->icon->setPixmap(pixmap);
}

void DeviceForm::setPercent(QString perct)
{
    ui->percentage->setText(perct);
}

void DeviceForm::setState(QString state)
{
    ui->state->setText(state);
}

void DeviceForm::setRemain(QString remain)
{
    ui->remaintext->setText(tr("RemainTime"));
    ui->remaindata->setText(remain);
}

void DeviceForm::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton)
    {
        QString cmd = QString("ukui-power-statistics --device %1 &").arg(path);
        qDebug()<<cmd;
        system(cmd.toStdString().c_str());
    }

}

void DeviceForm::widget_property_change()
{
    setIcon(icon_name);
    setPercent(percentage);
    slider_changed(percent_number);

    setState(state_text);
    setRemain(predict);
}

void DeviceForm::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}

 void DeviceForm::enterEvent(QEvent *event)
 {
     Q_UNUSED(event);
     setStyleSheet(
                 "background:rgba(61,107,229,1);"
                 "border-radius:2px;"
     );
 }

 void DeviceForm::leaveEvent(QEvent *event)
 {
     Q_UNUSED(event);
     setStyleSheet(
                 "QWidget {background-color:rgba(19,19,20,90%);}"
                 "QProgressBar {"
                 "width:284px;"
                 "height:4px;"
                 "background:rgba(255,255,255,0.1);"
                 "border-radius:2px;}"

     );
 }


 QString DeviceForm::calculate_value(int nValue,int nTotal)
 {
     QString strStyle = "";
     int value1 = nTotal * 0.2;
     if (nValue > value1)
     {

         if (nValue > (value1 * 2))
         {

             strStyle = QString("rgba(61,107,229,1)");

         }
         else
         {

             strStyle = QString("rgba(248,163,76,1)");

         }
     }
     else
     {
         strStyle = "rgba(240, 65, 52, 1)";
     }
     return strStyle;
 }

void DeviceForm::set_device(DEVICE *dev)
{

    if(dev == nullptr)
        return;
    m_device = dev;
    path = dev->m_dev.path;
    slot_device_change(dev);
    connect(ed,SIGNAL(signal_device_change(DEVICE*)),this,SLOT(slot_device_change(DEVICE*)));

//    connect(m_device,SIGNAL(device_property_changed(QDBusMessage,QString)),this,SLOT(device_change_slot()));
}

void DeviceForm::slot_device_change(DEVICE* device)
{
    if(device == nullptr)
        return;
    if(device->m_dev.path != path)
        return;
    icon_name = ed->engine_get_device_icon(device);
    percentage = QString::number(device->m_dev.Percentage, 'f',0)+"%";
    percent_number = int (device->m_dev.Percentage);
    state_text = ed->engine_kind_to_localised_text(device->m_dev.kind,0);
    predict = ed->engine_get_device_predict(device);
    widget_property_change();
}
 void DeviceForm::slider_changed(int value)
 {
     ui->progressBar->setValue(value);
     ui->progressBar->setStyleSheet(QString(""
         "	QProgressBar {"
         "	border-radius: 2px;"
         ""
         "}"
         "QProgressBar::chunk {"
             "border-radius:2px;"
             "	background-color: "
             "%1;"
         "}").arg(calculate_value(value,ui->progressBar->maximum())));
 }
