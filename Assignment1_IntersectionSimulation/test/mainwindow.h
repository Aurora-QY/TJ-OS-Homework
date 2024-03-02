
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

private slots:

    void on_posMoveBtn_clicked();   //pos

    void on_rectMoveBtn_clicked();  //geometry

    void on_opacityAnimationBtn_clicked();  //opacity

    void summon_label(int x=100,int y=100);

    void delBtn_clicked();

private:
    Ui::MainWindow *ui;

    QPropertyAnimation *m_animation;
};

#endif // MAINWINDOW_H
