#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <queue>
#include <random>
#include <vector>

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
    // 调整指令数量，使之为页大小的整数倍
    numLoadedPages = 0;
    numPageFaults = 0;
    FIFO_cnt = 0;

    for (int i = 0; i < MEMORY_SIZE; ++i) {
      MemoryIndex[i] = 0;
    }
    for (int i = 0; i < JOB_SIZE; ++i) {
      InstructionQueue[i] = 0;
    }
    // 随机生成指令访问次序
    GenerateInstructionQueue();
  }

  void GenerateInstructionQueue() {
    random_device rd;
    mt19937 gen(rd());
    int cnt = 0;
    int vis[JOB_SIZE];
    int randomqueue[JOB_SIZE];
    for (int i = 0; i < JOB_SIZE; ++i) {
      randomqueue[i] = i;
      vis[i] = false;
    }
    shuffle(randomqueue, randomqueue + JOB_SIZE, gen);
    for (int i = 0; i < JOB_SIZE; ++i) {
      if (!vis[randomqueue[i]]) {
        InstructionQueue[cnt++] = randomqueue[i];
        vis[randomqueue[i]] = true;
        if (rd() % 2 && !vis[(randomqueue[i] + 1) % JOB_SIZE]) { //一半的顺序
          InstructionQueue[cnt++] = (randomqueue[i] + 1) % JOB_SIZE;
          vis[(randomqueue[i] + 1) % JOB_SIZE] = true;
        }
      }
    }
  }

  void execute(const char *mode) {
    for (int i = 0; i < JOB_SIZE; ++i) {
      int instruction = InstructionQueue[i];
      int pageIndex = instruction / PAGE_SIZE;

      cout << "第" << i + 1 << "条指令的物理地址为 " << InstructionQueue[i]
           << endl;
      pages[pageIndex].last_used = i; //更新“上一次使用”时间为第i个指令
      if (!pages[pageIndex].loaded) {
        HandlePageFault(pageIndex, mode);
        ++numPageFaults; //缺页数+1
        faultsRate = double(numPageFaults) / (i + 1);

      } else {
        for (int i = 0; i < MEMORY_SIZE; ++i) {
          if (MemoryIndex[i] == pageIndex) {
            cout << i << "号块有" << pageIndex << "号指令" << endl;
          }
        }
      }
    }

    cout << "作业执行完毕，缺页率为 "
         << static_cast<double>(numPageFaults) / JOB_SIZE << endl;
  }

  void HandlePageFault(const int pageIndex, const char *mode) {

    int victimIndex; //被替换页面的序号
    if (!strcmp(mode, "FIFO")) {
      victimIndex = FIFO_cnt;
      FIFO_cnt = (FIFO_cnt + 1) % 4;
    } else {
      int last_used = INF; //要找出last_uesd最小（使用最久远）的页
      for (int i = 0; i < MEMORY_SIZE; ++i) {
        if (last_used > pages[MemoryIndex[i]].last_used) {
          last_used = pages[MemoryIndex[i]].last_used;
          victimIndex = i;
        }
      }
    }
    pages[MemoryIndex[victimIndex]].loaded = false;
    pages[pageIndex].loaded = true; //更改装载状态
    MemoryIndex[victimIndex] = pageIndex;
    cout << pageIndex << "装入" << victimIndex << endl;
  }

private:
  int numLoadedPages;             //已装载页面数
  int numPageFaults;              //缺页数
  Page pages[JOB_SIZE];           //页面容器
  int MemoryIndex[MEMORY_SIZE];   //记录
  int FIFO_cnt;                   //先进先出受害者指针
  int InstructionQueue[JOB_SIZE]; //任务序列
  double faultsRate;
};

int main() {
  Job job;
  string algorithm;
  while (1) {
    cout << "请输入所用算法:";
    cin >> algorithm;
    if (algorithm == "FIFO")
      job.execute("FIFO");
    else if (algorithm == "LRU")
      job.execute("LRU");
    else {
      cout << "输入错误" << endl;
    }
  }
  return 0;
}
