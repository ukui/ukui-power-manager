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
#include"DeviceUi.h"

void DeviceUi::uiInit(QWidget * DeviceForm)
{
    if (DeviceForm->objectName().isEmpty())
	DeviceForm->setObjectName(QString::fromUtf8("DeviceForm"));
    DeviceForm->resize(324, 82);
    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(DeviceForm->sizePolicy().
				 hasHeightForWidth());
    DeviceForm->setSizePolicy(sizePolicy);
    DeviceForm->setMinimumSize(QSize(324, 82));
    DeviceForm->setMaximumSize(QSize(324, 82));
    icon = new QLabel(DeviceForm);
    icon->setObjectName(QString::fromUtf8("icon"));
    icon->setGeometry(QRect(0, 0, 32, 32));
    kind = new QLabel(DeviceForm);
    kind->setObjectName(QString::fromUtf8("kind"));
    kind->setGeometry(QRect(41, 0, 131, 18));
    percentage = new QLabel(DeviceForm);
    percentage->setObjectName(QString::fromUtf8("percentage"));
    percentage->setGeometry(QRect(183, 1, 141, 11));
    percentage->
	setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::
		     AlignVCenter);
    remaintext = new QLabel(DeviceForm);
    remaintext->setObjectName(QString::fromUtf8("remaintext"));
    remaintext->setGeometry(QRect(41, 39, 91, 12));
    remaindata = new QLabel(DeviceForm);
    remaindata->setObjectName(QString::fromUtf8("remaindata"));
    remaindata->setGeometry(QRect(183, 35, 141, 16));
    remaindata->
	setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::
		     AlignVCenter);
    progressBar = new QProgressBar(DeviceForm);
    progressBar->setObjectName(QString::fromUtf8("progressBar"));
    progressBar->setGeometry(QRect(40, 24, 284, 4));
    progressBar->setValue(24);
    progressBar->setTextVisible(false);
    QMetaObject::connectSlotsByName(DeviceForm);
}
