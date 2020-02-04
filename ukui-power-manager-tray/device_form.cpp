#include "device_form.h"
#include "ui_deviceform.h"
#include <QIcon>
#include <QDebug>
#include <QStyleOption>
#include <QPainter>

DeviceForm::DeviceForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceForm)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground,true);

}

DeviceForm::~DeviceForm()
{
    delete ui;
}

void DeviceForm::setIcon(QString name)
{
    QIcon icon = QIcon::fromTheme(name);
    qDebug()<<icon.name()<<"-----------this is device icon---------------------";
    QPixmap pixmap = icon.pixmap(QSize(22,22));
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

void DeviceForm::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    QString cmd = "ukui-power-statistics-qt &";
    system(cmd.toStdString().c_str());

}

void DeviceForm::widget_property_change()
{
    setIcon(icon_name);
    setPercent(percentage);
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
     setStyleSheet(
                 "background:rgba(61,107,229,1);"
                 "border-radius:2px;"
     );
 }

 void DeviceForm::leaveEvent(QEvent *event)
 {

     setStyleSheet(
                 "background:rgba(14,19,22,0.75);"
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
             strStyle = QString("qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, \
                                stop:0 rgba(255, 0, 0, 255), stop:%1 rgba(255, 0, 0, 255), \
                                stop:%2 rgba(255, 255, 0, 255), stop:%3 rgba(255, 255, 0, 255) \
                                stop:%4 rgba(0, 0, 255, 255), stop:1 rgba(0, 0, 255, 255))")
                 .arg(str1)
                 .arg(str2)
                 .arg(str3)
                 .arg(str4);
         }
         else
         {
             strStyle = QString("qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 0, 0, 255), stop:%1 rgba(255, 0, 0, 255), stop:%2 rgba(255, 255, 0, 255), stop:1 rgba(255, 255, 0, 255))")
                 .arg(str1)
                 .arg(str2);
         }
     }
     else
     {
         strStyle = "rgba(255, 0, 0, 255)";
     }
     return strStyle;
 }


 void DeviceForm::slider_changed(int value)
 {
     ui->progressBar->setValue(value);
     ui->progressBar->setStyleSheet(QString(""
         "	QProgressBar {"
         "	border: 2px solid grey;"
         "	border-radius: 5px;"
         ""
         "}"

         "QProgressBar::chunk {"
             "	background-color: "
             "%1;"
         "}").arg(calculate_value(value,ui->progressBar->maximum())));
 }
