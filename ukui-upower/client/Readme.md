关于QDBusInterface QDBusReply的调用
有更多其他调用方式
例如在调用statusnotifier插件的每个应用的dbus时候

void Widget::on_pushButton_clicked()
{
    QDBusInterface iface("org.kde.StatusNotifierItem-2255-1",
                          "/StatusNotifierItem",
                          "org.freedesktop.DBus.Properties",
                         QDBusConnection::sessionBus());
    QDBusReply<QVariant> reply=iface.call("Get","org.kde.StatusNotifierItem","Id");
    qDebug()<<"reply  Id   is      :   "<<reply.value().toString();
}

