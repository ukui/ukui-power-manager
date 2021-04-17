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
#include"MainUi.h"

void MainUi::uiInit(QMainWindow *MainWindow)
{
    if (MainWindow->objectName().isEmpty())
        MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
    MainWindow->resize(388, 333);
    centralWidget = new QWidget(MainWindow);
    centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
    mainVerticalLayout = new QVBoxLayout(centralWidget);
    mainVerticalLayout->setSpacing(6);
    mainVerticalLayout->setContentsMargins(11, 11, 11, 11);
    mainVerticalLayout->setObjectName(QString::fromUtf8("mainVerticalLayout"));
    widget = new QWidget(centralWidget);
    widget->setObjectName(QString::fromUtf8("widget"));
    horizontalLayout = new QHBoxLayout(widget);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setContentsMargins(11, 11, 11, 11);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    horizontalLayout->setContentsMargins(0, 0, 0, 0);
    power_title = new QLabel(widget);
    power_title->setObjectName(QString::fromUtf8("power_title"));
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(power_title->sizePolicy().hasHeightForWidth());
    power_title->setSizePolicy(sizePolicy);

    horizontalLayout->addWidget(power_title);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer);

    verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(0);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    verticalSpacer_2 = new QSpacerItem(13, 5, QSizePolicy::Minimum, QSizePolicy::Fixed);

    verticalLayout->addItem(verticalSpacer_2);

    statistic_button = new QPushButton(widget);
    statistic_button->setObjectName(QString::fromUtf8("statistic_button"));
    statistic_button->setEnabled(true);
    sizePolicy.setHeightForWidth(statistic_button->sizePolicy().hasHeightForWidth());
    statistic_button->setSizePolicy(sizePolicy);

    verticalLayout->addWidget(statistic_button);

    verticalSpacer = new QSpacerItem(13, 2, QSizePolicy::Minimum, QSizePolicy::Fixed);

    verticalLayout->addItem(verticalSpacer);


    horizontalLayout->addLayout(verticalLayout);


    mainVerticalLayout->addWidget(widget);

    verticalSpacer_3 = new QSpacerItem(30, 30, QSizePolicy::Minimum, QSizePolicy::Fixed);

    mainVerticalLayout->addItem(verticalSpacer_3);

    listWidget = new QListWidget(centralWidget);
    listWidget->setObjectName(QString::fromUtf8("listWidget"));
    listWidget->setLineWidth(0);
    listWidget->setAutoScroll(false);
    listWidget->setAutoScrollMargin(0);
    listWidget->setItemAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    mainVerticalLayout->addWidget(listWidget);

    MainWindow->setCentralWidget(centralWidget);

    QMetaObject::connectSlotsByName(MainWindow);
}
