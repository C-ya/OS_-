#include <iostream>
#include <cstring>
#include <queue>
#include <stack>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iomanip>

using namespace std;


#define MAX 500
#define LENGTH 32   //虚存容量
#define orderNum 256 //指令个数
#define pageSize 4

int a[orderNum];//地址流
int pageno[orderNum];//页号
int Size;//页面大小
double ratio;//命中率

struct Page
{
    int num;
    bool pagefault; //是否在内存中
    int time; //时间
};
Page page[LENGTH];

/*
void init_a();
A. 在[0，256]的指令地址之间随机选取一起点 M；
B. 顺序执行一条指令，即执行地址为 M+1 的指令；
C. 在前地址[0，M+1]中随机选取一条指令并执行，该指令的地址为 M’；
D. 顺序执行一条指令，其地址为 M’+1；
E. 在后地址[M’+2，256]中随机选取一条指令并执行；
F. 重复A—E，直到执行 256 次指令。
*/
void init_a()//初始化地址流
{
    int firstM;
    srand((unsigned)time(NULL)); //随机种子
    for (int i = 0; i < orderNum; i += 6)
    {
        firstM = rand()%16384+16384;//[16384-32767]
        a[i] = firstM; //任选一指令节点
        a[i + 1] = a[i] + 1; //顺序访问下一条指令
        a[i + 2] = rand()%16384+16384; //前地址指令
        a[i + 3] = a[i + 2] + 1; //顺序访问下一条指令
        a[i + 4] = rand()%16384+16384; //后地址指令
        a[i + 5] = a[i + 4] + 1; //顺序访问下一条指令
    }
}

void init_p()
{
    int inisize=Size/4;
    for (int i = 0; i < orderNum; i++)
        pageno[i] = (a[i]/(1024*inisize))+1;//计算页号

    for (int i = 0; i < LENGTH; i++)
    {
        page[i].pagefault = 0;
        page[i].num = i;
        page[i].time = MAX - 1;
    }
}

void OPT()
{
    int clock = 1; //记录时间
    double totalnum = 0; //指令不在内存的次数
    for (int i = 0; i < orderNum; i++)
    {
        int numin = 0;
        for (int k = 0; k < LENGTH; k++){
            if (page[k].pagefault)
                numin++;
        }
        if (page[pageno[i]].pagefault == 1) //指令所在页在内存中
            page[pageno[i]].time = clock; //更新进入时间
        if (page[pageno[i]].pagefault == 0) //指令所在页不在内存中
        {
            if (numin < Size)
            {
                totalnum++; //缺页发生,加入内存
                page[pageno[i]].pagefault = 1;
            }
            else
            {
                bool n = false;
                int j, t;
                for (int k = 0; k < LENGTH; k++) //以后不使用
                {
                    if (page[k].pagefault == 1)
                    {
                        for (j = i + 1; j < orderNum; j++)
                        {
                            if (pageno[j] == page[k].num)
                                break;
                        }
                        if (j == orderNum)
                        {
                            t = k;
                            n = true;
                            break;
                        }
                    }
                }
                if (n)
                {
                    page[t].pagefault = 0;
                    totalnum++; //缺页发生,加入内存
                    page[pageno[i]].pagefault = 1;
                    continue;
                }
                for (int k = 0; k < LENGTH; k++) //最长时间以后才会使用
                {
                    if (page[k].pagefault == 1)
                    {
                        for (j = i + 1; j < orderNum; j++)
                        {
                            if (pageno[j] == page[k].num)
                            {
                                if (page[k].time <= j)
                                {
                                    page[k].time = j;
                                }
                            }
                        }
                    }
                }
                int min = 0;
                for (int k = 0; k < LENGTH; k++)
                {
                    if (page[k].pagefault == 1 && page[k].time > min)
                    {
                        t = k;
                        min = page[k].time;
                    }
                }
                page[t].pagefault = 0;
                totalnum++; //缺页发生,加入内存
                page[pageno[i]].pagefault = 1;
            }
        }
    }
    ratio = 1 - (totalnum / orderNum);
    cout << "失效次数：" << totalnum << endl;
    cout << "命中率：" << (int)(ratio * 100) << "%" << endl;
}

void LRU()
{
    int clock = 1; //记录时间
    double totalnum = 0; //指令不在内存的次数
    for (int i = 0; i < orderNum; i++)
    {
        int numin = 0;
        for (int k = 0; k < LENGTH; k++)
        {
            if (page[k].pagefault)
                numin++;
        }
        if (page[pageno[i]].pagefault == 1) //指令所在页在内存中
        {
            page[pageno[i]].time = clock; //更新进入时间
            clock++;
        }
        if (page[pageno[i]].pagefault == 0) //指令所在页不在内存中
        {
            if (numin < Size)
            {
                totalnum++; //缺页发生,加入内存
                page[pageno[i]].pagefault = 1;
                page[pageno[i]].time = clock;
                clock++;
            }
            else
            {
                int Max = MAX;
                int t;
                for (int k = 0; k < LENGTH; k++)
                {
                    if (page[k].pagefault && page[k].time < Max)
                    {
                        Max = page[k].time;
                        t = k;
                    }
                }
                page[t].pagefault = 0;
                totalnum++; //缺页发生,加入内存
                page[pageno[i]].pagefault = 1;
                page[pageno[i]].time = clock;
                clock++;
            }
        }
    }
    ratio = 1 - (totalnum / orderNum);
    cout << "失效次数： " << totalnum << endl;
    cout << "命中率： " << (int)(ratio * 100) << "%" << endl;
}

void FIFO()
{
    int clock = 1; //记录时间
    double totalnum = 0; //指令不在内存的次数
    for (int i = 0; i < orderNum; i++)
    {
        int numin = 0;
        for (int k = 0; k < LENGTH; k++)
        {
            if (page[k].pagefault)
                numin++;
        }
        if (page[pageno[i]].pagefault == 1)
            continue; //指令所在页在内存中

        if (page[pageno[i]].pagefault == 0) //指令所在页不在内存中
        {
            if (numin < Size)
            {
                totalnum++; //缺页发生,加入内存
                page[pageno[i]].pagefault = 1;
                page[pageno[i]].time = clock;
                clock++;
            }
            else
            {
                int Max = MAX;
                int t;
                for (int k = 0; k < 32; k++)
                {
                    if (page[k].pagefault && page[k].time < Max)
                    {
                        Max = page[k].time;
                        t = k;
                    }
                }
                page[t].pagefault = 0;
                totalnum++; //缺页发生,加入内存
                page[pageno[i]].pagefault = 1;
                page[pageno[i]].time = clock;
                clock++;
            }
        }
    }

    ratio = 1 - (totalnum / orderNum);
    cout << "失效次数： " << totalnum << endl;
    cout << "命中率： " << (int)(ratio * 100) << "%" << endl;
}

int main()
{
    string choose;
    //int Size;
    init_a();
    cout << "THE VIRTUAL ADDRESS STREAM AS FOLLOWS:" << endl;
    for (int i = 0; i < orderNum; i++){
        cout <<  "a[" << left <<  setw(3) << i << "]="<< left <<  setw(13)  << a[i];
        if((i+1)%4==0)
            cout << endl;
    }
    cout << endl << "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =" << endl;
    cout << "The algorithm is:";
    cin >> choose;
    while(choose != "quit"){
        if(choose[0] == 'o' || choose[0] == 'O'){
            Size=pageSize;
            for(int i=0; i<4; i++) {
                   // cout << Size << endl;
                init_p();
                cout << "PAGE NUMBER WITH SIZE " << Size/4 << "k FOR EACH ADDRESS IS:" << endl;
                for (int i = 0; i < orderNum; i++){
                    cout <<  "pageno[" << left <<  setw(3) << i << "]=" << left << setw(10) << pageno[i];
                    if((i+1)%4==0)
                        cout << endl;
                }
                cout << endl << "vmsize=" << LENGTH << "k" << "     " << "pagesize=" << Size/4 << "k" << endl;
                cout << "-----------------------------" << endl;
               // cout << left << setw(30) << "page assigned" << left << setw(50) << "pages_in/total references" << endl;
                OPT();
                Size*=2;
            }
            cout << endl << "End the result for opt" << endl;
            cout << "************************************************" << endl;
        }
        else if(choose[0] == 'l' || choose[0] == 'L'){
            Size=pageSize;
            for(int i=0; i<4; i++) {
                init_p();
                cout << "PAGE NUMBER WITH SIZE " << Size/4 << "k FOR EACH ADDRESS IS:" << endl;
                for (int i = 0; i < orderNum; i++){
                    cout <<  "pageno[" << left <<  setw(3) << i << "]=" << left << setw(10) << pageno[i];
                    if((i+1)%4==0)
                        cout << endl;
                }
                cout << endl << "vmsize=" << LENGTH << "k" << "     " << "pagesize=" << Size/4 << "k" << endl;
                cout << "-----------------------------" << endl;
               // cout << left << setw(30) << "page assigned" << left << setw(50) << "pages_in/total references" << endl;
                LRU();
                Size*=2;
            }
            cout << endl << "End the result for lru" << endl;
            cout << "************************************************" << endl;
        }
        else if(choose[0] == 'f' || choose[0] == 'F'){
            Size=pageSize;
            for(int i=0; i<4; i++) {
                init_p();
                cout << "PAGE NUMBER WITH SIZE " << Size/4 << "k FOR EACH ADDRESS IS:" << endl;
                for (int i = 0; i < orderNum; i++){
                    cout <<  "pageno[" << left <<  setw(3) << i << "]=" << left << setw(10) << pageno[i];
                    if((i+1)%4==0)
                        cout << endl;
                }
                cout << endl << "vmsize=" << LENGTH << "k" << "     " << "pagesize=" << Size/4 << "k" << endl;
                cout << "-----------------------------" << endl;
               // cout << left << setw(30) << "page assigned" << left << setw(50) << "pages_in/total references" << endl;
                FIFO();
                Size*=2;
            }
            cout << endl << "End the result for fifo" << endl;
            cout << "************************************************" << endl;
        }
        else
            cout << "input opt or lru or fifo or quit." << endl;
        cout << "The algorithm is:";
        cin >> choose;
    }
    return 0;
}
