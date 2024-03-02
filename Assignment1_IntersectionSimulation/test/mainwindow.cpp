
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->b1, SIGNAL(clicked()), this, SLOT(on_posMoveBtn_clicked()));
    connect(ui->b2, SIGNAL(clicked()), this, SLOT(summon_label()));
    //创建动画对象空间（初始化可以指定动画控件和动画需使用的动画属性名）
    m_animation = new QPropertyAnimation();
    m_animation->setTargetObject(ui->label);    //设置使用动画的控件
    m_animation->setEasingCurve(QEasingCurve::Linear); //设置动画效果
}

MainWindow::~MainWindow()
{
    delete m_animation;
    delete ui;
}

void MainWindow::on_posMoveBtn_clicked()
{
    //pos:按点移动的动画（移动）
    m_animation->setPropertyName("pos");    //指定动画属性名
    m_animation->setDuration(3000);    //设置动画时间（单位：毫秒）
    m_animation->setStartValue(ui->label->pos());  //设置动画起始位置在label控件当前的pos
    m_animation->setEndValue(ui->label->pos() + QPoint(200, 100)); //设置动画结束位置
    m_animation->start();   //启动动画
}

void MainWindow::on_rectMoveBtn_clicked()
{
    //geometry:按矩形的动画（移动和缩放）
    m_animation->setPropertyName("geometry");   //指定动画属性名
    m_animation->setDuration(3000);    //设置动画时间（单位：毫秒）
    m_animation->setStartValue(ui->label->rect());  //设置动画起始位置

    //获取控件初始的大小
    int width = ui->label->rect().width();
    int height = ui->label->rect().height();

    //设置动画步长值，以及在该位置时的长宽
    m_animation->setKeyValueAt(0.5, QRect(ui->label->pos() - QPoint(100, 50)
                                          ,QSize( width + 50, height + 35)));

    m_animation->setEndValue(QRect(ui->label->pos() - QPoint(100, 50)
                                   ,QSize( width, height))); //设置动画结束位置及其大小
    m_animation->start();   //启动动画
}

void MainWindow::on_opacityAnimationBtn_clicked()
{
    //windowOpacity：不透明度（注意该效果只对顶级窗口有效哦）
    m_animation->setTargetObject(this);     //重设动画使用对象
    m_animation->setPropertyName("windowOpacity");  //指定动画属性名
    m_animation->setDuration(2000);     //设置动画时间（单位：毫秒）

    //设置动画步长值，以及在该位置时显示的透明度
    m_animation->setKeyValueAt(0, 1);
    m_animation->setKeyValueAt(0.5, 0);
    m_animation->setKeyValueAt(1, 0);

    m_animation->setLoopCount(-1);  //当值为-1时，动画一直运行，直到窗口关闭
    m_animation->start();   //启动动画
}

void MainWindow::summon_label(int x,int y){
    QPushButton *nBtn = new QPushButton(this);
    QString btn_text = QString("Count%1").arg(ui->verticalLayout->count());
    nBtn->setText(btn_text);
    //往布局器中插入新按钮
    ui->verticalLayout->insertWidget(0, nBtn);
    //设置该按钮的槽函数，用于删除对应的控件
    connect(nBtn, &QPushButton::clicked, this, &MainWindow::delBtn_clicked);
}

void MainWindow::delBtn_clicked()
{
    QPushButton *Btn = qobject_cast<QPushButton *>(sender());
    ui->verticalLayout->removeWidget(Btn);
    delete Btn;
}
