#include "MemoryPage.h"

// �������ָ����ʴ���
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
            if (rd() % 2 && !vis[(randomqueue[i] + 1) % JOB_SIZE]) { //һ���˳��
                InstructionQueue[cnt++] = (randomqueue[i] + 1) % JOB_SIZE;
                vis[(randomqueue[i] + 1) % JOB_SIZE] = true;
            }
        }
    }
}

//ִ��ָ��
void Job::execute(const char *mode) {
    for (int i = 0; i < JOB_SIZE; ++i) {
        int instruction = InstructionQueue[i];
        int pageIndex = instruction / PAGE_SIZE;
        bool Fault=!pages[pageIndex].loaded;
        int victim = -1;
        cout << "��" << i + 1 << "��ָ��������ַΪ " << InstructionQueue[i]
             << endl;
        pages[pageIndex].last_used = i; //���¡���һ��ʹ�á�ʱ��Ϊ��i��ָ��
        if (Fault) {
            victim = HandlePageFault(pageIndex, mode);
            ++numPageFaults; //ȱҳ��+1
            faultsRate = numPageFaults / (i + 1);

            signal_FaultRate(numPageFaults,faultsRate);

        } else {
            for (int i = 0; i < MEMORY_SIZE; ++i) {
                if (MemoryIndex[i] == pageIndex) {
                    cout << i << "����" << pageIndex << endl;
                }
            }
        }
        signal_instruction(i,instruction,Fault,victim,pageIndex);
    }
}

    //���ȱҳ���⣨�û���
    //���ػ���ҳ�������ڴ�������
int Job::HandlePageFault(const int pageIndex, const char *mode) {
    int victimIndex; //���滻ҳ������
    if (!strcmp(mode, "FIFO")) {
        victimIndex = FIFO_cnt;
        FIFO_cnt = (FIFO_cnt + 1) % 4;
    } else {
        int last_used = INF; //Ҫ�ҳ�last_uesd��С��ʹ�����Զ����ҳ
        for (int i = 0; i < MEMORY_SIZE; ++i) {
            if (last_used > pages[MemoryIndex[i]].last_used) {
                last_used = pages[MemoryIndex[i]].last_used;
                victimIndex = i;
            }
        }
    }
    pages[MemoryIndex[victimIndex]].loaded = false;
    pages[pageIndex].loaded = true; //����װ��״̬
    MemoryIndex[victimIndex] = pageIndex;
    return victimIndex;
}
