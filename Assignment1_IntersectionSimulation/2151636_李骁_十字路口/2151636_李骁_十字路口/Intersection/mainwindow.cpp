
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    IT=new Intersection;
    ui->setupUi(this);

    ui->LightNorth->setAlignment(Qt::AlignCenter);
    ui->LightSouth->setAlignment(Qt::AlignCenter);
    ui->LightEast->setAlignment(Qt::AlignCenter);
    ui->LightWest->setAlignment(Qt::AlignCenter);

    //绑定按钮与函数
    connect(ui->AddVehicleNorth,&QPushButton::clicked,[this]{IT->AddVehicle(NORTH);});
    connect(ui->AddVehicleSouth,&QPushButton::clicked,[this]{IT->AddVehicle(SOUTH);});
    connect(ui->AddVehicleEast,&QPushButton::clicked,[this]{IT->AddVehicle(EAST);});
    connect(ui->AddVehicleWest,&QPushButton::clicked,[this]{IT->AddVehicle(WEST);});
    connect(ui->AddEmergencyNorth,&QPushButton::clicked,[this]{IT->AddVehicle(NORTH, EMERGENCY);});
    connect(ui->AddEmergencySouth,&QPushButton::clicked,[this]{IT->AddVehicle(SOUTH, EMERGENCY);});
    connect(ui->AddEmergencyEast,&QPushButton::clicked,[this]{IT->AddVehicle(EAST, EMERGENCY);});
    connect(ui->AddEmergencyWest,&QPushButton::clicked,[this]{IT->AddVehicle(WEST, EMERGENCY);});
    connect(ui->Add5,&QPushButton::clicked,[this]{    for(int i=NORTH;i<=WEST;++i){
        for(int j=0;j<5;++j){
            IT->AddVehicle(i);
        }
    }});
    connect(ui->Add2_E,&QPushButton::clicked,[this]{    for(int i=NORTH;i<=WEST;++i){
        for(int j=0;j<3;++j){
            IT->AddVehicle(i,EMERGENCY);
        }
    }});

    //实时显示车辆数及信号灯剩余秒数
    m_Timer = new QTimer(this);
    connect(m_Timer, &QTimer::timeout, this, [this]{this->DisplayVehicleNum();});
    connect(m_Timer, &QTimer::timeout, this, [this]{this->DisplayLightTimer();});

    /*connect(m_Timer, &QTimer::timeout, this, [this]{ui->labeln->setText(QString::number(IT->GetEmergencyNumOnRoad(NORTH)));});
    connect(m_Timer, &QTimer::timeout, this, [this]{ui->labels->setText(QString::number(IT->GetEmergencyNumOnRoad(SOUTH)));});
    connect(m_Timer, &QTimer::timeout, this, [this]{ui->labele->setText(QString::number(IT->GetEmergencyNumOnRoad(WEST)));});
    connect(m_Timer, &QTimer::timeout, this, [this]{ui->labelw->setText(QString::number(IT->GetEmergencyNumOnRoad(EAST)));});*/

    connect(m_Timer, &QTimer::timeout, this, [this]{
            if(IT->IsRoadAvailable(NORTH_EAST)){
                ui->NE->setText("否");
                IT->IsRoadAvailable(NORTH_EAST);
            }
        else{
            ui->NE->setText("是");
        }});
    connect(m_Timer, &QTimer::timeout, this, [this]{
        if(IT->IsRoadAvailable(SOUTH_EAST)){
            ui->SE->setText("否");
            IT->IsRoadAvailable(SOUTH_EAST);
        }
        else{
            ui->SE->setText("是");
        }});
    connect(m_Timer, &QTimer::timeout, this, [this]{
        if(IT->IsRoadAvailable(SOUTH_WEST)){
            ui->SW->setText("否");
            IT->IsRoadAvailable(SOUTH_WEST);
        }
        else{
            ui->SW->setText("是");
        }});
    connect(m_Timer, &QTimer::timeout, this, [this]{
        if(IT->IsRoadAvailable(NORTH_WEST)){
            ui->NW->setText("否");
            IT->IsRoadAvailable(NORTH_WEST);
        }
        else{
            ui->NW->setText("是");
        }});
    m_Timer->start(50);


    Light_Timer = new QTimer(this);

    connect(Light_Timer, &QTimer::timeout, [this]{
        remaining_time_NS--;
        remaining_time_EW--;});

    //开始按钮
    connect(ui->Begin, &QPushButton::clicked, this, &MainWindow::Begin);

    //其他线程向主线程发送更改ui信号
    IT->GoVehicleSignal([this](const int ID, const int from, const bool is_emergency, const bool is_first_half) {
        QMetaObject::invokeMethod(this, "DisplayGoVehicle", Q_ARG(int, ID), Q_ARG(int, from), Q_ARG(bool, is_emergency), Q_ARG(bool, is_first_half));
    });

    IT->LightSignal([this](const int from, const int color) {
        QMetaObject::invokeMethod(this, "ChangeLightColor", Q_ARG(int, from), Q_ARG(int, color));
    });
}

MainWindow::~MainWindow()
{
    delete IT;
    delete m_Timer;
    delete Light_Timer;
    delete ui;
}

void MainWindow::Begin(){
    static bool is_running=false;
    if(is_running){
        MainWindow *newWindow = new MainWindow(this);
        this->close();
        newWindow->show();
    }else{
        ui->Begin->setText("重启");
        IT->GoThread();
        Light_Timer->start(1000);//间隔是1s
    }
    is_running=!is_running;
}

//展示车辆数量
void MainWindow::DisplayVehicleNum(){
    ui->VehicleNumNorth->setText("普通车辆: " + QString::number(IT->GetVehicleNum(NORTH) - IT->GetEmergencyNumInQueue(NORTH)) + "\n紧急车辆: " + QString::number(IT->GetEmergencyNumInQueue(NORTH)));
    ui->VehicleNumSouth->setText("普通车辆: " + QString::number(IT->GetVehicleNum(SOUTH) - IT->GetEmergencyNumInQueue(SOUTH)) + "\n紧急车辆: " + QString::number(IT->GetEmergencyNumInQueue(SOUTH)));
    ui->VehicleNumEast->setText("普通车辆: " + QString::number(IT->GetVehicleNum(EAST) - IT->GetEmergencyNumInQueue(EAST)) + "\n紧急车辆: " + QString::number(IT->GetEmergencyNumInQueue(EAST)));
    ui->VehicleNumWest->setText("普通车辆: " + QString::number(IT->GetVehicleNum(WEST) - IT->GetEmergencyNumInQueue(WEST)) + "\n紧急车辆: " + QString::number(IT->GetEmergencyNumInQueue(WEST)));
}

//展示信号灯剩余时间
void MainWindow::DisplayLightTimer(){
    ui->LightNorth->setText(QString::number(int(remaining_time_NS)));
    ui->LightSouth->setText(QString::number(int(remaining_time_NS)));
    ui->LightEast->setText(QString::number(int(remaining_time_EW)));
    ui->LightWest->setText(QString::number(int(remaining_time_EW)));
}

//车辆运行ui界面动画修改
void MainWindow::DisplayGoVehicle(const int ID, const int from, const bool is_emergency, const bool is_first_half){
    int cur_x=0,cur_y=0,add_x=0,add_y=0;
    switch(from){
    case NORTH:
       cur_x=170,cur_y=130;
       add_x=0,add_y=40;
       break;
    case EAST:
       cur_x=250, cur_y=170;
       add_x=-40,add_y=0;
       break;
    case SOUTH:
       cur_x=210,cur_y=250;
       add_x=0,add_y=-40;
       break;
    case WEST:
       cur_x=130,cur_y=210;
       add_x=40,add_y=0;
       break;
    default:
       qDebug("Wrong direction!");
       break;
    }
    if(is_first_half){
       QLabel* v_label = new QLabel(this);
       vehicle_label.push_back(pair{v_label,ID});
       v_label->setText(QString::number(ID));
       v_label->setFixedSize(20, 20);
       v_label->setStyleSheet(QString("background-color: %1").arg(is_emergency ? "rgb(255, 0, 127)" : "rgb(0, 162, 232)"));
       v_label->move(cur_x, cur_y);
       v_label->show();

       QPropertyAnimation *v_animation = new QPropertyAnimation();
       v_animation->setTargetObject(v_label);
       v_animation->setEasingCurve(QEasingCurve::Linear);
       v_animation->setPropertyName("pos");
       v_animation->setDuration(IT->GetpassTime() * 500);
       v_animation->setStartValue(v_label->pos());
       v_animation->setEndValue(v_label->pos() + QPoint(add_x, add_y));
       v_animation->start();   //启动动画

       // 在动画结束后删除动画对象(label指针不用删)
       QTimer::singleShot(IT->GetpassTime() * 500 + 100, [=](){
           delete v_animation;
       });
    }

    else{
       auto v_label_iterator = find_if(vehicle_label.begin(), vehicle_label.end(), [ID](const auto& p) { return p.second == ID; });
       if(v_label_iterator == vehicle_label.end())
           return;
       QLabel* v_label=v_label_iterator->first;

       QPropertyAnimation *v_animation = new QPropertyAnimation();
       v_animation->setTargetObject(v_label);
       v_animation->setEasingCurve(QEasingCurve::Linear);
       v_animation->setPropertyName("pos");
       v_animation->setDuration(IT->GetpassTime() * 1000);
       v_animation->setStartValue(v_label->pos());
       v_animation->setEndValue(v_label->pos() + QPoint(add_x * 2, add_y * 2));
       v_animation->start();   //启动动画

       // 在动画结束后删除标签、双端队列中的元素、动画对象
       QTimer::singleShot(IT->GetpassTime() * 1000 + 100, [=](){
           delete v_label;
           delete v_animation;
           vehicle_label.erase(v_label_iterator);
       });
    }
}

//更改路灯颜色并设定剩余时间
void MainWindow::ChangeLightColor(const int from, const int color){
    QString LightColor = (color == RED ? "red" : (color == YELLOW ? "yellow" : "green"));
    double timer=(color == RED ? this->IT->GetLightTimer() : (color == YELLOW ? this->IT->GetLightTimer() * 0.25 : this->IT->GetLightTimer() * 0.75));
    if(from==NORTH_SOUTH){
       remaining_time_NS = timer;
       ui->LightNorth->setStyleSheet(QString("QLabel{"
                                             "font-weight: bold;"
                                             "border-radius: 10px;"
                                             "background-color: %1;"
                                             "}").arg(LightColor));
       ui->LightSouth->setStyleSheet(QString("QLabel{"
                                             "font-weight: bold;"
                                             "border-radius: 10px;"
                                             "background-color: %1;"
                                             "}").arg(LightColor));
    }else{
       remaining_time_EW = timer;
       ui->LightEast->setStyleSheet(QString("QLabel{"
                                            "font-weight: bold;"
                                             "border-radius: 10px;"
                                             "background-color: %1;"
                                             "}").arg(LightColor));
       ui->LightWest->setStyleSheet(QString("QLabel{"
                                            "font-weight: bold;"
                                             "border-radius: 10px;"
                                             "background-color: %1;"
                                             "}").arg(LightColor));
    }
}
