#include <QDBusArgument>
#include <QString>

#ifndef DBUS_DEMO_EXAMPLE_STRUCT
#define	DBUS_DEMO_EXAMPLE_STRUCT
struct dbus_demo_example_struct
{
    int drv_ID;
    QString name;
    QString full_name;
    int notify_mid;

    friend QDBusArgument &operator<<(QDBusArgument &argument, const dbus_demo_example_struct&mystruct)
    {
        argument.beginStructure();
        argument << mystruct.drv_ID << mystruct.name << mystruct.full_name << mystruct.notify_mid;
        argument.endStructure();
        return argument;
    }

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, dbus_demo_example_struct&mystruct)
    {
        argument.beginStructure();
        argument >> mystruct.drv_ID >> mystruct.name >> mystruct.full_name >> mystruct.notify_mid;
        argument.endStructure();
        return argument;
    }

};

Q_DECLARE_METATYPE(dbus_demo_example_struct)
#endif
