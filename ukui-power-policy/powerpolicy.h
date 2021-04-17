#ifndef POWERPOLICY_H
#define POWERPOLICY_H

#include <QObject>
#include <QList>
#include "dbus_struct.h"
#include <QDBusMessage>
#include "gsettings.h"

class PowerPolicy:public QObject {
    Q_OBJECT Q_CLASSINFO("D-Bus Interface", "ukui.power.policy")
  public:
    explicit PowerPolicy(QObject * parent = 0);

    int process(int option);
    int script_process(QString cmd);
    int process_shell(int option);
     Q_SIGNALS:void onbattery_change(bool);
    void ModeChanged(int);
    public Q_SLOTS:QVariantList return_variantlist();
    QString control(int opt);
    void onPropertiesSlot(QDBusMessage msg);
    void onbattery_change_slot(bool flag);
    bool change_auto_switch(bool flag);
  private:
    int dbus_integer;
    QString ret;
    QVariant dbus_variant;
    bool onbattery;
    bool enable_auto = false;
    int mode;
    int desire_status;
    int desire_mode;
};

#endif				// POWERPOLICY_H
