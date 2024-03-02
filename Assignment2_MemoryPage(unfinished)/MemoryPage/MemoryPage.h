
#ifndef MEMORYPAGE_H
#define MEMORYPAGE_H

#endif // MEMORYPAGE_H

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <queue>
#include <random>
#include <vector>

//Qt有关头文件
#include <QMetaObject>
#include <QObject>
#include <QString>

using namespace std;

#define INF 2147483647
#define MEMORY_SIZE 4 // 内存块数
#define PAGE_SIZE 10  // 页面大小
#define JOB_SIZE 320  // 作业大小（指令数）

// 定义页面类
class Page {
public:
    // int list[PAGE_SIZE];  //存储指令地址
    bool loaded;   //是否装载
    int last_used; //上次使用时间

    Page() {
        loaded = false;
        last_used = -1;
    }
};

// 定义作业类
class Job {

public:
    Job() {
        numLoadedPages = 0;
        numPageFaults = 0;
        FIFO_cnt = 0;

        for (int i = 0; i < MEMORY_SIZE; ++i) {
            MemoryIndex[i] = 0;
        }
        for (int i = 0; i < JOB_SIZE; ++i) {
            InstructionQueue[i] = 0;
        }
        GenerateInstructionQueue();
    }
    // 随机生成指令访问次序
    void GenerateInstructionQueue();

    //执行指令
    void execute(const char *mode);

    //解决缺页问题（置换）
    //返回换出页在任务内存块中序号
    int HandlePageFault(const int pageIndex, const char *mode);

    //处理指令信号量(序号、地址、是否缺页、换出页、换入页)
    using SignalType_Instruction = function<void(int, int, bool, int, int)>;
    //处理缺页信号量(缺页数、缺页率)
    using SignalType_FaultRate = function<void(int ,int)>;

    //向主进程发送更改ui界面的信号(列表添加已执行指令信息)
    void SignaInstruction(SignalType_Instruction signal) {
        signal_instruction = signal;
    }

    //向主进程发送更改ui界面的信号(缺页率)
    void SignalFaultRate(SignalType_FaultRate signal) {
        signal_FaultRate = signal;
    }

private:
    int numLoadedPages;             //已装载页面数
    int numPageFaults;              //缺页数
    Page pages[JOB_SIZE];           //页面容器
    int MemoryIndex[MEMORY_SIZE];   //记录
    int FIFO_cnt;                   //先进先出受害者指针
    int InstructionQueue[JOB_SIZE]; //任务序列
    double faultsRate;

    SignalType_Instruction signal_instruction;
    SignalType_FaultRate signal_FaultRate;
};
