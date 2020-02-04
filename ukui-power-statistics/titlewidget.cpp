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
    initControl();
    connectSlots();

}


// 初始化控件;
void TitleWidget::initControl()
{
//    m_pIcon = new QLabel;
    m_pTitleContent = new QLabel;
    m_pTitleContent->setObjectName("TitleContent");

    m_pButtonHelp = new QToolButton;
    m_pButtonClose = new QToolButton;

    m_pButtonHelp->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));

    m_help = new QLabel(m_pButtonHelp);
    m_close = new QLabel(m_pButtonClose);
    m_help->setObjectName("labelhelp");
    m_close->setObjectName("labelclose");
    m_help->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    m_close->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
//    m_help->setPixmap(QPixmap(":/resource/icon/__con.png"));
//    m_close->setPixmap(QPixmap(":/resource/icon/1px_delete_con.png"));

    QHBoxLayout* mylayout = new QHBoxLayout(this);
//    mylayout->addWidget(m_pIcon);
    mylayout->addWidget(m_pTitleContent);

    mylayout->addWidget(m_pButtonHelp);
    mylayout->addWidget(m_pButtonClose);

    mylayout->setContentsMargins(10, 0, 16, 0);
    mylayout->setSpacing(30);

    m_pTitleContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pTitleContent->setText(tr("Power Statistics-device infomation"));
    this->setFixedHeight(TITLE_HEIGHT);
    this->setWindowFlags(Qt::FramelessWindowHint);
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
    QPixmap titleIcon(filePath);
    m_pIcon->setPixmap(titleIcon.scaled(IconSize));
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
    // 是否设置标题透明;
//    if (!m_isTransparent)
//    {
//        //设置背景色;
//        QPainter painter(this);
//        QPainterPath pathBack;
//        pathBack.setFillRule(Qt::WindingFill);
//        pathBack.addRoundedRect(QRect(0, 0, this->width(), this->height()), 3, 3);
//        painter.setRenderHint(QPainter::Antialiasing, true);
//        painter.fillPath(pathBack, QBrush(QColor(m_colorR, m_colorG, m_colorB)));
//    }

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


