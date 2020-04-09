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
#include "titlewidget.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QFile>
#include <QMouseEvent>
#include <QStyleOption>
#include <QPainter>

//#include <QX11Info>
//#include <X11/Xlib.h>

TitleWidget::TitleWidget(QWidget *parent)
    : QWidget(parent)
    , m_colorR(153)
    , m_colorG(153)
    , m_colorB(153)
    , m_isPressed(false)
    , m_windowBorderWidth(0)
    , m_isTransparent(false)
{
    // 初始化;
    setWindowBorderWidth(20);
    initControl();
    connectSlots();

}


// 初始化控件;
void TitleWidget::initControl()
{
    m_pTitleContent = new QLabel(this);
    m_pTitleContent->setObjectName("TitleContent");

    m_pButtonHelp = new ToolButton(HELP);
    m_pButtonClose = new ToolButton(CLOSE);

    m_pButtonHelp->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
//    m_pButtonHelp->setObjectName("toolhelp");
//    m_pButtonClose->setObjectName("toolclose");
//    m_pButtonHelp->setIcon(QIcon(":/resource/icon/help.png"));
//    m_help = new QLabel(m_pButtonHelp);
//    m_close = new QLabel(m_pButtonClose);
//    m_help->setObjectName("labelhelp");
//    m_close->setObjectName("labelclose");
//    m_help->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
//    m_close->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));

    QHBoxLayout* mylayout = new QHBoxLayout(this);
    mylayout->setContentsMargins(0,0,0,0);
    mylayout->setSpacing(0);
    mylayout->addSpacing(10);
    mylayout->addWidget(m_pTitleContent);
    QSpacerItem *spacer = new QSpacerItem(100,TITLE_HEIGHT,QSizePolicy::Expanding,QSizePolicy::Fixed);
    mylayout->addSpacerItem(spacer);
    mylayout->addWidget(m_pButtonHelp);
    mylayout->addSpacing(4);
    mylayout->addWidget(m_pButtonClose);
    mylayout->addSpacing(3);

    m_pTitleContent->setText(tr("Power Information"));
    m_pTitleContent->setStyleSheet("QLabel {font-size:14px;color:black}");
    this->setFixedHeight(TITLE_HEIGHT);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
}

// 信号槽的绑定;
void TitleWidget::connectSlots()
{
    connect(m_pButtonHelp, SIGNAL(clicked()), this, SLOT(onButtonHelpClicked()));
    connect(m_pButtonClose, SIGNAL(clicked()), this, SLOT(onButtonCloseClicked()));
}


void TitleWidget::setBackgroundColor(int r, int g, int b, bool isTransparent)
{
    m_colorR = r;
    m_colorG = g;
    m_colorB = b;
    m_isTransparent = isTransparent;
    update();
}

// 设置标题栏图标;
void TitleWidget::setTitleIcon(QString filePath, QSize IconSize)
{
    Q_UNUSED(filePath);
    Q_UNUSED(IconSize);
//    QPixmap titleIcon(filePath);
//    m_pIcon->setPixmap(titleIcon.scaled(IconSize));
}

// 设置标题内容;
void TitleWidget::setTitleContent(QString titleContent, int titleFontSize)
{
    // 设置标题字体大小;
    QFont font = m_pTitleContent->font();
    font.setPointSize(titleFontSize);
    m_pTitleContent->setFont(font);
    // 设置标题内容;
    m_pTitleContent->setText(titleContent);
    m_titleContent = titleContent;
}

// 设置标题栏长度;
void TitleWidget::setTitleWidth(int width)
{
    this->setFixedWidth(width);
}

// 设置窗口边框宽度;
void TitleWidget::setWindowBorderWidth(int borderWidth)
{
    m_windowBorderWidth = borderWidth;
}

// 绘制标题栏背景色;
void TitleWidget::paintEvent(QPaintEvent *event)
{
    if (this->width() != (this->parentWidget()->width() - m_windowBorderWidth))
    {
        this->setFixedWidth(this->parentWidget()->width() - m_windowBorderWidth);
    }
    QWidget::paintEvent(event);
}

void TitleWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_isPressed = true;
    m_startMovePos = event->globalPos();

    return QWidget::mousePressEvent(event);
}

void TitleWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_isPressed = false;
    return QWidget::mouseReleaseEvent(event);
}

void TitleWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPressed)
    {
        QPoint movePoint = event->globalPos() - m_startMovePos;
        QPoint widgetPos = this->parentWidget()->pos();
        m_startMovePos = event->globalPos();
        this->parentWidget()->move(widgetPos.x() + movePoint.x(), widgetPos.y() + movePoint.y());
//        moveWindow();
    }
    return QWidget::mouseMoveEvent(event);
}

void TitleWidget::moveWindow(void)
{
//    Display *display = QX11Info::display();
//    Atom netMoveResize = XInternAtom(display, "_NET_WM_MOVERESIZE", False);
//    XEvent xEvent;
//    const auto pos = QCursor::pos();

//    memset(&xEvent, 0, sizeof(XEvent));
//    xEvent.xclient.type = ClientMessage;
//    xEvent.xclient.message_type = netMoveResize;
//    xEvent.xclient.display = display;
//    xEvent.xclient.window = this->winId();
//    xEvent.xclient.format = 32;
//    xEvent.xclient.data.l[0] = pos.x();
//    xEvent.xclient.data.l[1] = pos.y();
//    xEvent.xclient.data.l[2] = 8;
//    xEvent.xclient.data.l[3] = Button1;
//    xEvent.xclient.data.l[4] = 0;

//    XUngrabPointer(display, CurrentTime);
//    XSendEvent(display, QX11Info::appRootWindow(QX11Info::appScreen()),
//               False, SubstructureNotifyMask | SubstructureRedirectMask,
//               &xEvent);
//    XFlush(display);
}

// 加载本地样式文件;
void TitleWidget::loadStyleSheet(const QString &sheetName)
{
    QFile file(":/Resources/" + sheetName + ".qss");
    file.open(QFile::ReadOnly);
    if (file.isOpen())
    {
        QString styleSheet = this->styleSheet();
        styleSheet += QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
    }
}

void TitleWidget::onButtonHelpClicked()
{
    emit signalButtonHelpClicked();
}

void TitleWidget::onButtonCloseClicked()
{
    emit signalButtonCloseClicked();
}


ToolButton::ToolButton(ButtonType type)
{
    mType = type;
    switch (type) {
    case HELP:
        setStyleSheet("QToolButton {border-radius:4px;background-color:#E7E7E7;}");
        setIcon(QIcon(":/resource/icon/help-symbolic.png"));
        break;
    case CLOSE:
        setStyleSheet("QToolButton {border-radius:4px;background-color:#E7E7E7;}");
        setIcon(QIcon(":/resource/icon/close.png"));
        break;
    default:
        break;
    }

}

void ToolButton::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons()==Qt::LeftButton)
    {
        setStyleSheet("QToolButton {border-radius:4px;background-color:#3257CA;}");
        if(mType == HELP)
            setIcon(QIcon(":/resource/icon/help-symbolic_white.png"));
        else
            setIcon(QIcon(":/resource/icon/closeWhite.png"));

    }
    QToolButton::mousePressEvent(event);
}

void ToolButton::enterEvent(QEvent *event)
{
    setStyleSheet("QToolButton {border-radius:4px;background-color:#3D6BE5;}");
    if(mType == HELP)
        setIcon(QIcon(":/resource/icon/help-symbolic_white.png"));
    else
        setIcon(QIcon(":/resource/icon/closeWhite.png"));
    QToolButton::enterEvent(event);
}

void ToolButton::leaveEvent(QEvent *event)
{
    setStyleSheet("QToolButton {border-radius:4px;background-color:#E7E7E7;}");
    if(mType == HELP)
        setIcon(QIcon(":/resource/icon/help-symbolic.png"));
    else
        setIcon(QIcon(":/resource/icon/close.png"));
    QToolButton::leaveEvent(event);

}

DeviceWidget::DeviceWidget(QWidget *parent):QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(5);
    layout->setContentsMargins(0,0,0,0);
    icon = new QLabel(this);
    content = new QLabel(this);
    layout->addWidget(icon);
    layout->addWidget(content);
    layout->addStretch();
    setLayout(layout);
//    setFixedSize(150,36);
}

//void DeviceWidget::paintEvent(QPaintEvent *event)
//{
//    Q_UNUSED(event);
//    QStyleOption opt;
//    opt.init(this);
//    QPainter p(this);
//    style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,this);
//}

void DeviceWidget::set_device_icon(QString name)
{
    icon->setPixmap(QPixmap(name));
}

void DeviceWidget::set_device_text(bool flag,QString text)
{
    if(!text.isEmpty())
    {
        m_text = text;
    }
    QFont font((content->font()));
    font.setPixelSize(14);
    QFontMetrics metric(font);
    QString metric_text = metric.elidedText(m_text,Qt::ElideRight,120);
    content->setText(metric_text);
    if(flag)
        content->setStyleSheet("QLabel {color:white}");
    else
        content->setStyleSheet("QLabel {color:black}");
}
