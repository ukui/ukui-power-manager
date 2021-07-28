#include "device_widget.h"

device_widget::device_widget(QWidget *parent) : QWidget(parent)
{
    initUI();
}

void device_widget::initUI()
{

    m_IconBatterChangeList   << KYLIN_BATTERY_CHARGING0_PATH  << KYLIN_BATTERY_CHARGING10_PATH
                             << KYLIN_BATTERY_CHARGING20_PATH << KYLIN_BATTERY_CHARGING30_PATH
                             << KYLIN_BATTERY_CHARGING40_PATH << KYLIN_BATTERY_CHARGING50_PATH
                             << KYLIN_BATTERY_CHARGING60_PATH << KYLIN_BATTERY_CHARGING70_PATH
                             << KYLIN_BATTERY_CHARGING80_PATH << KYLIN_BATTERY_CHARGING90_PATH
                             << KYLIN_BATTERY_CHARGING100_PATH <<KYLIN_BATTERY_00_PATH;

    mainLayout = new QHBoxLayout();

    deviceIconLabel = new QLabel();
    deviceNameLabel = new QLabel();
    percentageLabel = new QLabel();
    batteryLabel    = new QLabel();

    mainLayout->addWidget(deviceIconLabel);
    mainLayout->addWidget(deviceNameLabel);
    mainLayout->addItem(new QSpacerItem(300,5));
    mainLayout->addWidget(percentageLabel);
    mainLayout->addWidget(batteryLabel);

    this->setLayout(mainLayout);
    this->setFixedSize(328,48);


}

void device_widget::initDevice(DEVICE *dev)
{
    setIcon(dev->m_dev.Type);
    deviceNameLabel->setText(dev->m_dev.Type);
    percentageLabel->setText(QString("%1%").arg(dev->m_dev.Percentage));
    setBatteryIcon(dev->m_dev.Percentage);

}

void device_widget::setIcon(QString arg)
{
    if (arg == "鼠标") {
        QImage *img1 = new QImage; //新建一个image对象
        img1->load(":/charging/mouse/0.png");
        deviceIconLabel->setPixmap(QPixmap::fromImage(*img1));
    } else if (arg == "键盘") {
        QImage *img2 = new QImage; //新建一个image对象
        img2->load(":/charging/keyboard/0.png");
        deviceIconLabel->setPixmap(QPixmap::fromImage(*img2));
    }

}
void device_widget::setBatteryIcon(int Ele_surplus_int)
{
    QPixmap pixmap;
    if (Ele_surplus_int>=0 && Ele_surplus_int<10){
        pixmap = m_IconBatterChangeList.at(0);
    } else if (Ele_surplus_int>=10 && Ele_surplus_int<20){
        pixmap = m_IconBatterChangeList.at(1);
    } else if (Ele_surplus_int>=20 && Ele_surplus_int<30){
        pixmap = m_IconBatterChangeList.at(2);
    }else if (Ele_surplus_int>=30 && Ele_surplus_int<40){
        pixmap = m_IconBatterChangeList.at(3);
    }else if (Ele_surplus_int>=40 && Ele_surplus_int<50){
        pixmap = m_IconBatterChangeList.at(4);
    }else if (Ele_surplus_int>=50 && Ele_surplus_int<60){
        pixmap = m_IconBatterChangeList.at(5);
    }else if (Ele_surplus_int>=60 && Ele_surplus_int<70){
        pixmap = m_IconBatterChangeList.at(6);
    }else if (Ele_surplus_int>=70 && Ele_surplus_int<80){
        pixmap = m_IconBatterChangeList.at(7);
    }else if (Ele_surplus_int>=80 && Ele_surplus_int<90){
        pixmap = m_IconBatterChangeList.at(8);
    }else if (Ele_surplus_int>=90 && Ele_surplus_int<100){
        pixmap = m_IconBatterChangeList.at(9);
    }else if (Ele_surplus_int==100){
        pixmap = m_IconBatterChangeList.at(10);
    }
    batteryLabel->setPixmap(pixmap);

}

void device_widget::onBatteryChanged(QStringList args)
{
    //qDebug()<<"其他组件状态"<<args;
    int battery = args.at(0).toInt();
//    int state = args.at(1).toInt();
    setBatteryIcon(battery);
    percentageLabel->setText(QString("%1%").arg(battery));
}

void device_widget::paintEvent(QPaintEvent *e)
{

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    QRect rect = this->rect();
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(opt.palette.color(QPalette::Base));
//     p.setBrush(QColor(255,255,255));
    p.setOpacity(0.7);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect,6,6);
}
