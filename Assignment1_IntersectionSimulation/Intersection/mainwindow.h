
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Intersection.h>
#include <QTimer>
#include "ui_mainwindow.h"
#include <QPropertyAnimation>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void Begin();

    void DisplayVehicleNum();

    void DisplayLightTimer();

    //这里要声明为槽函数
    Q_SLOT void DisplayGoVehicle(const int ID, const int from, const bool is_emergency, const bool is_first_half);

    Q_SLOT void ChangeLightColor(const int from, const int color);


private:
    Ui::MainWindow *ui;

    Intersection *IT;

    QTimer *m_Timer, *Light_Timer;

    double remaining_time_NS,remaining_time_EW;

    deque<pair<QLabel*,int>> vehicle_label;

};

#endif // MAINWINDOW_H
