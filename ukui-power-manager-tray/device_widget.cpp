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
#include "device_widget.h"
#include "ui_devicewidget.h"
#include <QDebug>
#include <QStyleOption>
#include <QPainter>

DeviceWidget::DeviceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceWidget)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground,true);
}

DeviceWidget::~DeviceWidget()
{
    delete ui;
}

void DeviceWidget::setIcon(QString name)
{
    QIcon icon = QIcon::fromTheme(name);
    qDebug()<<icon.name()<<"-----------this is device icon---------------------";
    ui->btnicon->setIcon(icon);
}

void DeviceWidget::setPercent(QString perct)
{
    ui->percentage->setText(perct);
}

void DeviceWidget::setState(QString state)
{
    ui->state->setText(state);
}

void DeviceWidget::setRemain(QString remain)
{
    ui->remaintext->setText(tr("RemainTime"));
    ui->remaindata->setText(remain);
}

void DeviceWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    QString cmd = "ukui-power-statistics-qt &";
    system(cmd.toStdString().c_str());

}

void DeviceWidget::widget_property_change()
{
    setIcon(icon_name);
    setPercent(percentage);
    setState(state_text);
    setRemain(predict);
}

void DeviceWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}

 void DeviceWidget::enterEvent(QEvent *event)
 {
     setStyleSheet(
                 "background:rgba(61,107,229,1);"
                 "border-radius:2px;"
     );
 }

 void DeviceWidget::leaveEvent(QEvent *event)
 {

     setStyleSheet(
                 "background:rgba(14,19,22,0.75);"
     );
 }
