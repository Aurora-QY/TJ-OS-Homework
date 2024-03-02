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
#define MEMORY_SIZE 4 // �ڴ����
#define PAGE_SIZE 10  // ҳ���С
#define JOB_SIZE 320  // ��ҵ��С��ָ������

// ����ҳ����
class Page {
public:
  // int list[PAGE_SIZE];  //�洢ָ���ַ
  bool loaded;   //�Ƿ�װ��
  int last_used; //�ϴ�ʹ��ʱ��

  Page() {
    loaded = false;
    last_used = -1;
  }
};

// ������ҵ��
class Job {

public:
  Job() {
    // ����ָ��������ʹ֮Ϊҳ��С��������
    numLoadedPages = 0;
    numPageFaults = 0;
    FIFO_cnt = 0;

    for (int i = 0; i < MEMORY_SIZE; ++i) {
      MemoryIndex[i] = 0;
    }
    for (int i = 0; i < JOB_SIZE; ++i) {
      InstructionQueue[i] = 0;
    }
    // �������ָ����ʴ���
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
        if (rd() % 2 && !vis[(randomqueue[i] + 1) % JOB_SIZE]) { //һ���˳��
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

      cout << "��" << i + 1 << "��ָ��������ַΪ " << InstructionQueue[i]
           << endl;
      pages[pageIndex].last_used = i; //���¡���һ��ʹ�á�ʱ��Ϊ��i��ָ��
      if (!pages[pageIndex].loaded) {
        HandlePageFault(pageIndex, mode);
        ++numPageFaults; //ȱҳ��+1
        faultsRate = double(numPageFaults) / (i + 1);

      } else {
        for (int i = 0; i < MEMORY_SIZE; ++i) {
          if (MemoryIndex[i] == pageIndex) {
            cout << i << "�ſ���" << pageIndex << "��ָ��" << endl;
          }
        }
      }
    }

    cout << "��ҵִ����ϣ�ȱҳ��Ϊ "
         << static_cast<double>(numPageFaults) / JOB_SIZE << endl;
  }

  void HandlePageFault(const int pageIndex, const char *mode) {

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
    cout << pageIndex << "װ��" << victimIndex << endl;
  }

private:
  int numLoadedPages;             //��װ��ҳ����
  int numPageFaults;              //ȱҳ��
  Page pages[JOB_SIZE];           //ҳ������
  int MemoryIndex[MEMORY_SIZE];   //��¼
  int FIFO_cnt;                   //�Ƚ��ȳ��ܺ���ָ��
  int InstructionQueue[JOB_SIZE]; //��������
  double faultsRate;
};

int main() {
  Job job;
  string algorithm;
  while (1) {
    cout << "�����������㷨:";
    cin >> algorithm;
    if (algorithm == "FIFO")
      job.execute("FIFO");
    else if (algorithm == "LRU")
      job.execute("LRU");
    else {
      cout << "�������" << endl;
    }
  }
  return 0;
}
