#include "widget.h"
#include "ui_widget.h"
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("客户端");
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    QDBusInterface iface("org.ukui.power",
                         "/",
                         "org.ukui.power",
                         QDBusConnection::sessionBus());
    QDBusReply<QString> reply=iface.call("UPowerVersion");
    qDebug()<<reply;
}
