
#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
        connect(ui->RFIFO, &QPushButton::clicked, this, [this]{job->execute("FIFO");});
        connect(ui->RLRU, &QPushButton::clicked, this, [this]{job->execute("LRU");});
    // 创建内容窗口
    QWidget* contentWidget = new QWidget(ui->list);
    ui->list->setWidget(contentWidget);

    // 创建网格布局
    QGridLayout* layout = new QGridLayout(contentWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);  // 可选：设置控件之间的间距

    job->SignalFaultRate([this](const int num, const int rate){
        this->ui->NumFault->setText(QString("%1").arg(num));
        this->ui->RateFault->setText(QString("%1").arg(rate));
    });

    job->SignaInstruction([&layout, &contentWidget, this](const int ID, const int instruction, const bool fault, const int outpage ,const int inpage ) {
        this->showIns(layout,contentWidget, ID, instruction, fault, outpage ,inpage);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::showFault(){

}

void MainWindow::showIns(QGridLayout *layout, QWidget* contentWidget,const int ID, const int instruction, const bool fault, const int outpage ,const int inpage){
    QLabel *label_ID = new QLabel(QString("%1").arg(ID), contentWidget);
    label_ID->setFixedSize(40, 12);  // 设置标签的固定大小
    layout->addWidget(label_ID, list_row, 0);
    QLabel *label_instruction = new QLabel(QString("%1").arg(instruction), contentWidget);
    label_instruction->setFixedSize(40, 12);  // 设置标签的固定大小
    layout->addWidget(label_instruction, list_row, 1);
    QLabel *label_fault = new QLabel(fault?"是":"否");
    label_fault->setFixedSize(40, 12);  // 设置标签的固定大小
    layout->addWidget(label_fault, list_row, 2);
    QLabel *label_outpage = new QLabel(QString("%1").arg(outpage), contentWidget);
    label_outpage->setFixedSize(40, 12);  // 设置标签的固定大小
    layout->addWidget(label_outpage, list_row, 3);
    QLabel *label_inpage = new QLabel(QString("%1").arg(inpage), contentWidget);
    label_inpage->setFixedSize(40, 12);  // 设置标签的固定大小
    layout->addWidget(label_inpage, list_row, 4);
    list_row++;
}
