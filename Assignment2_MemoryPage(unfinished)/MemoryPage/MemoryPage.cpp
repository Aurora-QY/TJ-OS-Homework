#include "MemoryPage.h"

// 随机生成指令访问次序
void Job::GenerateInstructionQueue() {
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

//执行指令
void Job::execute(const char *mode) {
    for (int i = 0; i < JOB_SIZE; ++i) {
        int instruction = InstructionQueue[i];
        int pageIndex = instruction / PAGE_SIZE;
        bool Fault=!pages[pageIndex].loaded;
        int victim = -1;
        cout << "第" << i + 1 << "条指令的物理地址为 " << InstructionQueue[i]
             << endl;
        pages[pageIndex].last_used = i; //更新“上一次使用”时间为第i个指令
        if (Fault) {
            victim = HandlePageFault(pageIndex, mode);
            ++numPageFaults; //缺页数+1
            faultsRate = numPageFaults / (i + 1);

            signal_FaultRate(numPageFaults,faultsRate);

        } else {
            for (int i = 0; i < MEMORY_SIZE; ++i) {
                if (MemoryIndex[i] == pageIndex) {
                    cout << i << "号有" << pageIndex << endl;
                }
            }
        }
        signal_instruction(i,instruction,Fault,victim,pageIndex);
    }
}

    //解决缺页问题（置换）
    //返回换出页在任务内存块中序号
int Job::HandlePageFault(const int pageIndex, const char *mode) {
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
    return victimIndex;
}
