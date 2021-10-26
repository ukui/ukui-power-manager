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


#include "widget.h"
#include "ui_widget.h"
#include "dbus.h"
#include "dbus-ukuipower.h"
#include <QDebug>
#include <QDBusConnection>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("服务端");
    register_dbus();

}

Widget::~Widget()
{
    delete ui;
}

void Widget::register_dbus()
{
    DBus* dbus=new DBus;
    new UpowerAdaptor(dbus);
    QDBusConnection con=QDBusConnection::sessionBus();
    if(!con.registerService("org.ukui.upower") ||
            !con.registerObject("/",dbus,
                                QDBusConnection::ExportAllSlots|
                                QDBusConnection::ExportAllSignals))
    {
        qDebug()<<"fail";
    }
}
