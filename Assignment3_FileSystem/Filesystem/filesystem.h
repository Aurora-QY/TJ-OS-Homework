#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "ui_filesystem.h"
#include <QtWidgets/QMainWindow>

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;
#define FILE_PATH "./FileSystem.dat" // 默认文件路径在项目根目录下

#define BLOCK_SIZE 512                        // 模拟磁盘块大小(字节)
#define BLOCK_NUM 128                         // 模拟磁盘块数
const int DISK_SIZE = BLOCK_SIZE * BLOCK_NUM; // 磁盘大小

#define MAX_NAME 19 // 最长文件名

enum FILE_TYPE { // FCB的type
  EMPTY,         // 空闲
  MYFILE,        // 文件
  DIRECTORY,     // 目录
};

enum FAT_STATE { // FAT存储数值代表含义(在这之外的表示文件下一个盘块地址)
  FREE, // 空闲
  FILE_END,
  //已分配，该块为文件结尾(创建文件初始化使用，等到写入长度过长再更改值为下一个块地址)
  DIR, // 已分配为目录
};

enum ERROR { //创建与删除文件/目录的返回信息
  SUCCESS,   //成功
  //创建文件
  DUPLICATED, //重名
  DISK_FULL,  //磁盘已满
  DIR_FULL,   //当前目录已满
  //删除文件
  NOT_EMPTY, //当前目录非空
};

enum Path { //更改路径类别
  FORWARD,  //进入子目录
  BACK,     //返回上一级
};

////长度：19+1+4+4+4=32
struct FCB {
  char name[MAX_NAME]; //文件名
  char type;           //文件类型
  int size;            //文件大小
  int ParentAddress;   //当前的上级目录地址(物理块号)
  int Address;         //当前目录地址

  FCB(const char *name = "\0", int type = EMPTY, int Address = 0,
      int ParentAddress = 0, int size = 0) {
    strcpy(this->name, name);
    this->type = type;
    this->size = size;
    this->Address = Address;
    this->ParentAddress = ParentAddress;
  }
};

const int MAX_DIRECTORY = BLOCK_SIZE / sizeof(FCB) - 1; //目录文件的最多FCB数

struct Directory //目录结构
{
  FCB DirectoryFCB; //自身FCB
  FCB SonFCB[MAX_DIRECTORY];

  Directory(const char *name = "\0", int Address = 0, int ParentAddress = 0) {
    DirectoryFCB =
        FCB(name, DIRECTORY, Address, ParentAddress, DirectoryFCB.size);
    for (int i = 0; i < MAX_DIRECTORY; ++i) { //子FCB的上级目录地址为当前块号
      SonFCB[i] = FCB("\0", EMPTY, 0, Address, 0);
    }
  }
};
QT_BEGIN_NAMESPACE
namespace Ui {
class fileSystem;
}
QT_END_NAMESPACE

struct Disk {
  int FAT[BLOCK_NUM];    // FAT
  struct Directory root; //根目录
  char data[BLOCK_NUM - 2][BLOCK_SIZE];

  static const int ROOT = 1;

  Disk() {
    memset(FAT, 0, sizeof(FAT));
    FAT[0] = FAT[1] = DIR; // 0,1盘块号依次代表FAT,根目录区
    root = Directory("\\root", ROOT, ROOT); //根目录区
    memset(data, '\0', sizeof(data));
  }
};

class fileSystem : public QMainWindow {
  Q_OBJECT

public:
  //数据操作部分：

  void Format();
  //格式化
  void SaveFS();
  //保存文件系统
  bool LoadFS();
  //载入文件系统
  //返回值：是否读入成功

  Directory *LocateAddress(const int dir_num);
  // 根据指定块号的目录定位并返回对应目录指针(默认盘块号为当前目录)
  // 返回值：指定块号的目录对应目录指针(根目录或转换为*Directory的物理块)
  int FindSon(Directory *dir, const char *name, const char type);
  // 查找到当前目录下对应名称、对应类别的SonFCB对应下标
  // 返回值：查找成功则返回盘块号，查找失败则返回-1

  int Create(const char *NewName, const char type);
  // 在当前目录下创建目录/文件
  // 返回值：创建状态信息(0为成功，1为当前目录下有同名且同类别的文件，2为磁盘已满，3为当前目录已满)
  bool Delete(const char *DelName, const char type);
  // 删除当前目录下的目录/文件
  // 返回值：删除是否成功

  bool ChangePath(const char type, const char *DirName = "\0");
  //更改当前路径，type为更改类别(0为进入，1为返回上一级)
  //返回值：是否更改路径成功(已在根目录时不能返回上一级)

  string ReadFile(const FCB *OpenFCB);
  //查看文件内容

  bool WriteFile(const char *content, FCB *OpenFCB); //修改文件

  //界面绘制部分：

  void DrawCell(const int row, const int col, FCB *SonFCB);
  //绘制文件列表中单元格：目录/文件
  void DrawTable();
  //绘制文件列表界面
  void DrawBitMap(const Directory *dir);
  //绘制文件占用位图

  fileSystem(QWidget *parent = NULL);
  ~fileSystem() {
    //析构函数中，即程序退出时存储文件
    SaveFS();
    delete ui;
  };

public slots:                //槽函数
  void CreateDirectoryFuc(); //新建文件夹
  void CreateFileFuc();      //新建文件

private:
  Disk *pFile = new Disk();  //虚拟磁盘指针
  FILE *fp = new FILE();     // fstream操作文件地址
  char *Disk_Address = NULL; //虚拟磁盘空间基地址
  int used_block = 2;        //已用物理块数量

  Ui::fileSystem *ui;
};
#endif // FILESYSTEM_H
