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
    setAttribute(Qt::WA_StyledBackground,true);
    ed = EngineDevice::getInstance();
//    connect(ed,SLOT(signal_device_change(DEVICE*)),this,SLOT(slot_device_change(DEVICE*)));
}

DeviceForm::~DeviceForm()
{
    delete ui;
}

void DeviceForm::setIcon(QString name)
{
    if(name.contains("charging"))
    {

    }
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
        QString cmd = "ukui-power-statistics &";
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
                 "background:rgba(14,19,22,0.90);"
     );
 }


 QString DeviceForm::calculate_value(int nValue,int nTotal)
 {
     QString strStyle = "";
     int value1 = nTotal * 0.3 + 0.5;
     if (nValue > value1)
     {
         QString str1;
         QString str2;
         float scale1 = (float)value1 / (float)nValue;
         float scale2 = scale1 + 0.000004;
         str1 = str1.setNum(scale1,'f',6);
         str2 = str2.setNum(scale2,'f',6);

         if (nValue > (value1 * 2))
         {
             QString str3;
             QString str4;

             float scale3 = 2.0 * scale1;
             float scale4 = scale3 + 0.000004;
             str3 = str3.setNum(scale3,'f',6);
             str4 = str4.setNum(scale4,'f',6);
             qDebug() << str3 << str4;
//             strStyle = QString("qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, \
//                                stop:0 rgba(255, 0, 0, 255), stop:%1 rgba(255, 0, 0, 255), \
//                                stop:%2 rgba(255, 255, 0, 255), stop:%3 rgba(255, 255, 0, 255) \
//                                stop:%4 rgba(0, 0, 255, 255), stop:1 rgba(0, 0, 255, 255))")
//                 .arg(str1)
//                 .arg(str2)
//                 .arg(str3)
//                 .arg(str4);
             strStyle = QString("rgba(61,107,229,1)");

         }
         else
         {
//             strStyle = QString("qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 0, 0, 255), stop:%1 rgba(255, 0, 0, 255), stop:%2 rgba(255, 255, 0, 255), stop:1 rgba(255, 255, 0, 255))")
//                 .arg(str1)
//                 .arg(str2);
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
