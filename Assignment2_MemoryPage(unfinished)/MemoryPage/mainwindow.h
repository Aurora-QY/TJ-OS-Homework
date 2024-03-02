
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "MemoryPage.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QThread>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void showIns(QGridLayout *layout, QWidget* contentWidget,const int ID, const int instruction, const bool fault, const int outpage ,const int inpage);

    void showFault();

private:
    Ui::MainWindow *ui;
    Job *job;

    int list_row=0;
};

#endif // MAINWINDOW_H
