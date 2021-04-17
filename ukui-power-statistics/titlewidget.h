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
#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>

#define BUTTON_HEIGHT 30        // 按钮高度;
#define BUTTON_WIDTH 30         // 按钮宽度;
#define TITLE_HEIGHT 36         // 标题栏高度;

class TitleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TitleWidget(QWidget *parent);

    void setBackgroundColor(int r, int g, int b , bool isTransparent = false);
    void setTitleIcon(QString filePath , QSize IconSize = QSize(25 , 25));
    void setTitleContent(QString titleContent , int titleFontSize = 9);
    void setTitleWidth(int width);
    void setWindowBorderWidth(int borderWidth);

private:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    // 初始化控件;
public:

    void initControl();
    void connectSlots();
    // 加载样式文件;
    void loadStyleSheet(const QString &sheetName);

Q_SIGNALS:
    // 按钮触发的信号;

    void signalButtonHelpClicked();
    void signalButtonCloseClicked();

private Q_SLOTS:
    void onButtonHelpClicked();
    void onButtonCloseClicked();
private:
    QLabel* m_pTitleContent;            // 标题栏内容;
    QToolButton* m_pButtonHelp;          // 最大化按钮;
    QToolButton* m_pButtonClose;        // 关闭按钮;
    QLabel* m_help;                    // 标题栏图标;
    QLabel* m_close;            // 标题栏内容;
    // 标题栏背景色;
    int m_colorR;
    int m_colorG;
    int m_colorB;

    // 移动窗口的变量;
    bool m_isPressed;
    QPoint m_startMovePos;
    // 标题栏内容;
    QString m_titleContent;
    // 窗口边框宽度;
    int m_windowBorderWidth;
    // 标题栏是否透明;
    bool m_isTransparent;
};

enum ButtonType
{
    HELP,
    CLOSE,
};

class ToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit ToolButton(ButtonType type);
protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
private:
    ButtonType mType;
};

class DeviceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceWidget(QWidget *parent=nullptr);
protected:
//    virtual void paintEvent(QPaintEvent *event);
private:
    QLabel *icon;
    QLabel *content;
    QString m_text;
public:
    void set_device_icon(QString name);
    void set_device_text(bool flag,QString text = "");
};

#endif // TITLEWIDGET_H
