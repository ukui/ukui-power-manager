#ifndef MAINUI_H
#define MAINUI_H
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>


class MainUi {
  public:
    QWidget * centralWidget;
    QVBoxLayout *mainVerticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLabel *power_title;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer_2;
    QPushButton *statistic_button;
    QSpacerItem *verticalSpacer;
    QSpacerItem *verticalSpacer_3;
    QListWidget *listWidget;

    void uiInit(QMainWindow * MainWindow);

};


#endif				// MAINUI_H
