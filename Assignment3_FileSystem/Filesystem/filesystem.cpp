#include "filesystem.h"
#include "ui_filesystem.h"

#include <QDebug>
#include <QHeaderView>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>

QString CurrentPath = "\\root";    //当前路径
int CurrentDirectory = Disk::ROOT; //当前目录的盘块号

//消息对话框
void messageBox(const char *s) { QMessageBox::information(NULL, "提示", s); }

fileSystem::fileSystem(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::fileSystem) {
  ui->setupUi(this);
  if (LoadFS())
    messageBox("检测到已保存磁盘，读入成功");
  else
    messageBox("未检测到已保存磁盘，已新建");
  QFont font("Arial", 5);
  QTableWidgetItem *FATitem = new QTableWidgetItem("FAT表");
  FATitem->setBackground(Qt::gray);
  FATitem->setFont(font);
  ui->BitMap->setItem(0, 0, FATitem);
  QTableWidgetItem *Rootitem = new QTableWidgetItem("根目录");
  Rootitem->setBackground(Qt::green);
  Rootitem->setFont(font);
  ui->BitMap->setItem(0, 1, Rootitem);

  ui->PathLabel->setText(CurrentPath);
  DrawTable();

  // 绑定按钮与函数
  connect(ui->FormatBtn, &QPushButton::clicked, this, [=]() {
    Format();
    ui->PathLabel->setText(CurrentPath);
    DrawTable();
    messageBox("格式化成功");
  });
  connect(ui->BackBtn, &QPushButton::clicked, this, [=]() {
    if (!ChangePath(BACK))
      messageBox("已在根目录下，无法返回");
    ui->PathLabel->setText(CurrentPath);
    DrawTable();
    ;
  });
  connect(ui->AddFileBtn, &QPushButton::clicked, this,
          &fileSystem::CreateFileFuc);
  connect(ui->AddDirBtn, &QPushButton::clicked, this,
          &fileSystem::CreateDirectoryFuc);
}

// 格式化
void fileSystem::Format() {
  CurrentDirectory = Disk::ROOT;
  CurrentPath = "\\root"; //当前路径
  *pFile = Disk();        //打开文件列表初始化
  fp = fopen(FILE_PATH, "w+");
  fwrite(Disk_Address, sizeof(char), DISK_SIZE, fp);
  fclose(fp);
}

// 保存文件系统
void fileSystem::SaveFS() {
  fp = fopen(FILE_PATH, "w+");
  fwrite(Disk_Address, sizeof(char), DISK_SIZE, fp);
  fclose(fp);
  free(pFile);
}

// 载入文件系统
// 返回值：是否读入成功
bool fileSystem::LoadFS() {
  Disk_Address = (char *)malloc(DISK_SIZE); //申请虚拟空间并且初始化
  pFile = (Disk *)(Disk_Address);           //虚拟磁盘初始化
  fp = fopen(FILE_PATH, "r");
  if (fp != NULL) { //加载磁盘文件
    fread(Disk_Address, sizeof(char), DISK_SIZE, fp);
    return true;
  } else {
    Format();
    return false;
  }
}

// 根据指定块号的目录定位并返回对应目录指针(默认盘块号为当前目录)
// 返回值：指定块号的目录对应目录指针(根目录或转换为*Directory的物理块)
Directory *fileSystem::LocateAddress(const int dir_num = CurrentDirectory) {
  Directory *dir = NULL;     //当前目录的指针
  if (dir_num == Disk::ROOT) // 根目录
    dir = &(pFile->root);
  else
    dir = (Directory *)(pFile->data[dir_num - 2]);
  return dir;
}

// 查找到当前目录下对应名称、对应类别的SonFCB对应下标
// 返回值：查找成功则返回盘块号，查找失败则返回-1
int fileSystem::FindSon(Directory *dir, const char *name, const char type) {
  for (int i = 0; i < MAX_DIRECTORY; ++i) {
    if (dir->SonFCB[i].type == type && !strcmp(dir->SonFCB[i].name, name)) {
      return i;
    }
  }
  return -1;
}

// 在当前目录下创建目录/文件
// 返回值：创建状态信息(0为成功，1为当前目录下有同名且同类别的文件，2为磁盘已满，3为当前目录已满)
int fileSystem::Create(const char *NewName, char type) {
  Directory *dir = LocateAddress();

  //查找目录下是否有同名文件夹
  if (FindSon(dir, NewName, type) != -1)
    return DUPLICATED;
  //查找空闲FCB
  for (int i = 0; i < MAX_DIRECTORY; ++i) {
    if (dir->SonFCB[i].type == EMPTY) {
      //查找到空闲FCB，查找空闲物理块
      for (int j = 2; j < BLOCK_NUM; ++j) { //从FAT[2]开始搜索,即除去FAT和根目录
        if (!pFile->FAT[j]) {               // FAT为空闲
          dir->SonFCB[i] = FCB(NewName, type, j, CurrentDirectory);
          pFile->FAT[j] = (type == DIRECTORY ? DIR : FILE_END);
          if (type == DIRECTORY) {
            Directory *NewDir =
                (Directory *)(pFile->data[j - 2]); // 对应的字块需要-2
            *NewDir = Directory(NewName, j, CurrentDirectory);
          } //上级目录设置为当前目录，位置设置为j(即找到的空闲物理块)
          ++used_block; //已用物理块+1
          return SUCCESS;
        }
      }
      return DISK_FULL;
    }
  }
  return DIR_FULL;
}

//删除当前目录下的目录/文件
//返回值：删除是否成功
bool fileSystem::Delete(const char *DelName, const char type) {
  Directory *dir = LocateAddress();

  int target = FindSon(dir, DelName, type); //对应下标
  if (target == -1) {
    qDebug() << "指定目录/文件不存在！";
    return false;
  }

  int address = dir->SonFCB[target].Address;

  if (type == DIRECTORY) { //如果删除目标为目录，则还需要考虑其子目录
    Directory *DelDir =
        (struct Directory *)(pFile->data[address - 2]); //查找到目标对应文件块
    for (int i = 0; i < MAX_DIRECTORY; i++) { //查找目标目录是否为空目录
      if (DelDir->SonFCB[i].type != EMPTY) {
        messageBox("该文件夹为非空文件夹,为确保安全，请清空后再删除!");
        return false;
      }
    }
  }

  if (type ==
      MYFILE) { //如果删除目标为文件，则还需要删除其后续文件(FAT置零即可，不需要后续删除)
    int next_Address = pFile->FAT[dir->SonFCB[target].Address];
    //如果该块不是文件结尾
    while (next_Address != FILE_END) {
      int tmp = next_Address;
      next_Address = pFile->FAT[next_Address]; //找到后继文件
      pFile->FAT[tmp] = FREE;
    }
  }

  //更新位图
  pFile->FAT[address] = FREE; //清空FAT
  dir->SonFCB[target] = FCB(); //当前目录中，目标目录/文件对应FCB初始化
  char *p = pFile->data[address - 2]; //格式化对应物理块
  memset(p, '\0', BLOCK_SIZE);
  --used_block; //已用物理块-1
  return true;
}

//更改当前路径，type为更改类别(0为进入，1为返回上一级)
//返回值：是否更改路径成功(已在根目录时不能返回上一级)
bool fileSystem::ChangePath(const char type, const char *DirName) {
  Directory *dir = LocateAddress();
  //返回上一级
  if (type == BACK) {
    if (CurrentDirectory == Disk::ROOT) {
      return false;
    }
    CurrentDirectory = dir->DirectoryFCB.ParentAddress;
    int lastSlashIndex =
        CurrentPath.lastIndexOf('\\'); // 查找最后一个反斜杠的索引
    if (lastSlashIndex != -1) {
      CurrentPath.remove(lastSlashIndex, CurrentPath.length() - lastSlashIndex);
      //删除最后一个反斜杠及其后面的内容
    }
    return true;
  }
  //进入子目录
  int target = FindSon(dir, DirName, DIRECTORY);
  if (target == -1) {
    qDebug() << "指定子目录不存在！";
    return false;
  }
  //修改当前文件信息
  CurrentDirectory = dir->SonFCB[target].Address;
  CurrentPath += "\\";
  CurrentPath += dir->SonFCB[target].name;
  return true;
}

//绘制文件列表中单元格：目录/文件
void fileSystem::DrawCell(const int row, const int col, FCB *SonFCB) {
  string content;

  QPushButton *Btn = new QPushButton;
  if (SonFCB->type == DIRECTORY) {
    Btn->setText(QString(SonFCB->name));
    Btn->setStyleSheet("QPushButton {"
                       "  border-image: url(:/icons/Dir1.png);"
                       "}"
                       "QPushButton:hover {"
                       "  border-image: url(:/icons/Dir2.png);"
                       "}"
                       "QPushButton:pressed {"
                       "  border-image: url(:/icons/Dir3.png);"
                       "}");

    // 创建右键菜单
    QMenu *menu = new QMenu(Btn);
    QAction *enterAction = new QAction("进入", menu);
    QAction *deleteAction = new QAction("删除", menu);
    menu->addAction(enterAction);
    menu->addAction(deleteAction);
    menu->setStyleSheet("QMenu::item:selected {"
                        "background-color:blue;"
                        "color:#FFFFFF;"
                        "}");

    // 关联按钮和右键菜单
    Btn->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(Btn, &QPushButton::customContextMenuRequested,
            [=](const QPoint &pos) { menu->exec(Btn->mapToGlobal(pos)); });
    connect(Btn, &QPushButton::clicked, enterAction, &QAction::triggered);

    connect(enterAction, &QAction::triggered, [=]() {
      ChangePath(FORWARD, SonFCB->name);   // 更改路径
      ui->PathLabel->setText(CurrentPath); // 更新路径
      DrawTable();
    });

    connect(deleteAction, &QAction::triggered, [=]() {
      if (Delete(SonFCB->name, DIRECTORY))
        messageBox("删除成功");
      else
        messageBox("删除失败，请检查目标目录是否为空");
      DrawTable();
    });
  } else if (SonFCB->type == MYFILE) {
    Btn->setText(QString(SonFCB->name) + "\n" + QString::number(SonFCB->size) +
                 "B");
    Btn->setStyleSheet("QPushButton {"
                       "  border-image: url(:/icons/File1.png);"
                       "}"
                       "QPushButton:hover {"
                       "  border-image: url(:/icons/File2.png);"
                       "}"
                       "QPushButton:pressed {"
                       "  border-image: url(:/icons/File3.png);"
                       "}");

    // 创建右键菜单
    QMenu *menu = new QMenu(Btn);
    QAction *viewAction = new QAction("查看", menu);
    QAction *modifyAction = new QAction("修改", menu);
    QAction *deleteAction = new QAction("删除", menu);
    menu->addAction(viewAction);
    menu->addAction(modifyAction);
    menu->addAction(deleteAction);
    menu->setStyleSheet("QMenu::item:selected {"
                        "background-color:blue;"
                        "color:#FFFFFF;"
                        "}");

    // 关联按钮和右键菜单，以及关联按钮单击事件
    Btn->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(Btn, &QPushButton::customContextMenuRequested,
            [=](const QPoint &pos) { menu->exec(Btn->mapToGlobal(pos)); });
    connect(Btn, &QPushButton::clicked, viewAction, &QAction::triggered);

    connect(viewAction, &QAction::triggered, [=]() {
      string defaultInput = ReadFile(SonFCB);
      QInputDialog *Dialog = new QInputDialog(this);
      Dialog->setOptions(QInputDialog::NoButtons);
      Dialog->getMultiLineText(this, //弹出文本框
                               "文件内容", "", defaultInput.c_str());
    });

    connect(modifyAction, &QAction::triggered, [=]() {
      string defaultInput = ReadFile(SonFCB);
      bool bOk = false;
      QString text = QInputDialog::getMultiLineText(this, //弹出输入文本框
                                                    "修改..", "请输入内容",
                                                    defaultInput.c_str(), &bOk);
      if (bOk) {                                               //点击确定
        WriteFile((char *)text.toStdString().c_str(), SonFCB); //修改文件内容
      }
      DrawTable();
    });

    connect(deleteAction, &QAction::triggered, [=]() {
      if (Delete(SonFCB->name, MYFILE))
        messageBox("删除成功");
      else
        qDebug() << ("删除错误");
      DrawTable();
    });
  } else
    qDebug() << "插入视图出错！";
  //插入按钮
  ui->tableWidget->setCellWidget(row, col, Btn);
}

//绘制文件列表界面
void fileSystem::DrawTable() {
  for (int i = 2; i < BLOCK_NUM; ++i) {
    QTableWidgetItem *item = new QTableWidgetItem("");
    item->setBackground(Qt::white);
    ui->BitMap->setItem(i / 8, i % 8, item);
  }
  DrawBitMap(LocateAddress(Disk::ROOT));
  Directory *dir = LocateAddress();

  int SonNum = 0;
  //先显示目录
  for (int i = 0; i < MAX_DIRECTORY; ++i) {
    if (dir->SonFCB[i].type == EMPTY)
      continue;
    if (dir->SonFCB[i].type == DIRECTORY) { //查找目录文件
      DrawCell(SonNum / 4, SonNum % 4, &dir->SonFCB[i]);
      ++SonNum;
    }
  }
  //再显示文件
  for (int i = 0; i < MAX_DIRECTORY; ++i) {
    if (dir->SonFCB[i].type == EMPTY)
      continue;
    if (dir->SonFCB[i].type == MYFILE) { //查找普通文件
      DrawCell(SonNum / 4, SonNum % 4, &dir->SonFCB[i]);
      ++SonNum;
    }
  }

  for (int i = SonNum; i < 16; ++i) //清空多余单元格(自动delete旧单元格内容)
    ui->tableWidget->setCellWidget(i / 4, i % 4, new QLabel());
}

//绘制文件占用位图
void fileSystem::DrawBitMap(const Directory *dir) {
  for (int i = 0; i < MAX_DIRECTORY; ++i) {
    if (dir->SonFCB[i].type == EMPTY)
      continue;
    QTableWidgetItem *item = new QTableWidgetItem(dir->SonFCB[i].name);
    QFont font("Arial", 5);
    item->setFont(font);
    if (dir->SonFCB[i].type == DIRECTORY) {
      item->setBackground(Qt::cyan);
      DrawBitMap(LocateAddress(dir->SonFCB[i].Address));
    } else {
      item->setBackground(Qt::yellow);
      int next_Address = pFile->FAT[dir->SonFCB[i].Address];
      //如果该块不是文件结尾
      while (next_Address != FILE_END) {
        QTableWidgetItem *additem = new QTableWidgetItem(dir->SonFCB[i].name);
        QFont font("Arial", 5);
        additem->setFont(font);
        additem->setBackground(Qt::lightGray);
        ui->BitMap->setItem(next_Address / 8, next_Address % 8, additem);
        next_Address = pFile->FAT[next_Address]; //找到后继文件
      }
    }
    ui->BitMap->setItem(dir->SonFCB[i].Address / 8, dir->SonFCB[i].Address % 8,
                        item);
  }
}

bool fileSystem::WriteFile(const char *content,
                           FCB *OpenFCB) { //在指定的文件里记录信息
  char *p = pFile->data[OpenFCB->Address - 2];
  int len = strlen(content);               //输入内容长度
  const int len_per_file = BLOCK_SIZE - 1; //每个文件能存储的最大长度
  int file_num = (len + len_per_file - 1) / len_per_file; //需要的文件数量
  if (file_num > BLOCK_NUM - used_block) { //如果要新建的文件长度超出空闲长度
    messageBox("磁盘剩余空间不足！");
    return false;
  }

  //先把之前的后续文件全部置空
  int next_Address = pFile->FAT[OpenFCB->Address];
  int tmp_address;
  while (next_Address != FILE_END) {
    tmp_address = next_Address;
    next_Address = pFile->FAT[next_Address]; //找到后继文件
    pFile->FAT[tmp_address] = FREE;
  }
  //很重要!!!!!
  pFile->FAT[OpenFCB->Address] = FILE_END;

  OpenFCB->size = len;
  used_block = file_num;

  //先填写开始的当前文件
  strncpy(p, content, len_per_file); //拷贝长度为BLOCK_SIZE-1=511的字符串
  p[len_per_file] = '\0';            //最后附上一个尾零

  char *temp_p = NULL;
  int temp_address = OpenFCB->Address; //存储上一个填写的块地址

  //注意i=1开始，填入后续文件
  for (int i = 1; i < file_num; ++i) {
    for (int j = 2; j < BLOCK_NUM; ++j) { //从FAT[2]开始搜索,即除去FAT和根目录
      if (!pFile->FAT[j]) {               // FAT为空闲
        temp_p = pFile->data[j - 2];  // 当前文件指针
        pFile->FAT[j] = FILE_END;     // 标记为文件结尾
        pFile->FAT[temp_address] = j; // 前一块的后继为当前块
        //按块装入
        strncpy(temp_p, content + i * len_per_file,
                len_per_file);  //拷贝长度为BLOCK_SIZE-1=511的串到目标
        p[len_per_file] = '\0'; //最后附上一个尾零
        temp_address = j;
        break; //注意这个break
      }
    }
  }
  messageBox("成功修改文件！");
  return true;
}

//查看文件内容
string fileSystem::ReadFile(const FCB *OpenFCB) {
  char *p = pFile->data[OpenFCB->Address - 2];
  string content;
  int next_Address = OpenFCB->Address;

  do { //未到文件结尾
    content += p;
    next_Address = pFile->FAT[next_Address]; //找到后继文件地址
  } while (next_Address != FILE_END);
  return content;
}

void fileSystem::CreateDirectoryFuc() {
  bool isOK;
  QString text = QInputDialog::getText(NULL, "输入目录名...",
                                       "请输入要创建的目录名(18个char以内)",
                                       QLineEdit::Normal, "目录", &isOK);
  if (isOK) {
    //增加文件
    string strText = text.toStdString();
    char *dirName = (char *)strText.c_str();
    switch (Create(dirName, DIRECTORY)) {
    case SUCCESS:
      break;
    case DUPLICATED:
      messageBox("当前目录下已有重名目录");
      break;
    case DISK_FULL:
      messageBox("磁盘已满");
      break;
    case DIR_FULL:
      messageBox("当前目录已满");
      break;
    }
  }
  DrawTable();
}

void fileSystem::CreateFileFuc() {
  bool isOK;
  QString text = QInputDialog::getText(NULL, "输入文件名...",
                                       "请输入要创建的文件名(18个char以内)",
                                       QLineEdit::Normal, "文件", &isOK);
  if (isOK) {
    //增加文件
    string strText = text.toStdString();
    char *fileName = (char *)strText.c_str();
    switch (Create(fileName, MYFILE)) {
    case SUCCESS:
      break;
    case DUPLICATED:
      messageBox("当前目录下已有重名文件");
      break;
    case DISK_FULL:
      messageBox("磁盘已满");
      break;
    case DIR_FULL:
      messageBox("当前目录已满");
      break;
    }
  }
  DrawTable();
}
