#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>
#include <stack>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <typeinfo>
using namespace std;
/******************************************************************** 实验中定义的相关结构体：RE、NFA、DFA、RG及函数声明 ***********/
/************************************************************************* NFA *********************************************/
enum sign
{
    left = 0,
    ad,
    mul,
    cycle
};
typedef struct NFANode
{
    int num;   // 编号
    char name; // 顶点命名

} NFANode, *INFA;

typedef struct NFA
{
    string name;                           // NFA的名字
    string re;                             // 输入的正则表达式
    int stateNum;                          // 状态数
    INFA S;                                // 指向开始符号
    vector<INFA> T;                        // 指向终结符号的集合
    vector<vector<pair<int, char>>> graph; // NFA状态关系图，以邻接链表混合邻接矩阵方式存储，int表示指向哪个结点，char表示这条边的输入。
                                           // 第一层vector相当于一个数组，数组里存的每个元素对应这某个编号对应的所有边，如vector[0]相当于是编号为0的点对应的所有从0出发的边，如<0,1>,<0,2>
    // 第二层vector存储对应的边指向的结点和边的输入。如vector[0]里存储了了pair<7,'1'>,相当于有边<0,7>，输入为1
    //
    //
} NFA;

struct NfaState /*定义NFA状态*/
{

    int index; /*NFA状态的状态号*/ // 猜测可能需要在输出的地方修改为q index+1输出

    char input; /*NFA状态弧上的值*/ // 猜测可能需要改变变量类型为int,目前没有修改因为char用来接收符号
    int chTrans;                    /*NFA状态弧转移到的状态号*/

    set<int> epTrans; /*当前状态通过ε转移到的状态号集合*/
};

struct NFA1
{

    NfaState *head; /*NFA的头指针*/
    NfaState *tail; /*NFA的尾指针*/
};

static NfaState NfaStates[128]; /*NFA状态数组*/
static int nfaStateNum = 0;     /*NFA状态总数*/

void add(NfaState *n1, NfaState *n2, char ch);
void add(NfaState *n1, NfaState *n2);
NFA1 creatNFA(int sum);
void insert(string &s, int n, char ch);
void preprocess(string &s);
int priority(char ch);
string infixToSuffix(string s);
set<int> epcloure(set<int> s);
set<int> moveEpCloure(set<int> s, char ch);
bool IsEnd(NFA1 n, set<int> s);
NFA1 strToNfa(string s);

void transfer_RE(string &re, NFA &nfa); // 生成NFA的主函数
NFA transfer(NFA1 &temp);

/************************************************************************* DFA ***********************************************/
typedef struct DFANode
{
    int num;   // 编号
    char name; // 顶点命名
} DFANode, *IDFA;

typedef struct DFA
{
    string name;                           // DFA的名字
    int stateNum;                          // 状态数
    IDFA S;                                // 指向开始符号
    vector<IDFA> T;                        // 指向终结符号的集合
    vector<vector<pair<int, char>>> graph; // DFA状态关系图，以邻接链表混合邻接矩阵方式存储，int表示指向哪个结点，char表示这条边的输入。
    // 第一层vector相当于一个数组，数组里存的每个元素对应这某个编号对应的所有边，如vector[0]相当于是编号为0的点对应的所有从0出发的边，如<0,1>,<0,2>
    // 第二层vector存储对应的边指向的结点和边的输入。如vector[0]里存储了了pair<7,'1'>,相当于有边<0,7>，输入为1
    vector<vector<bool>> flag; // 可区分状态表
    vector<vector<int>> node;  // 二维数组，将等价DFA都放在一行
} DFA, DFA_MIN;

void transfer_NFA(NFA &nfa, DFA &dfa);
bool Isequal(set<int> a, set<int> b);

/************************************************************************* DFA min *********************************************/
void transfer_DFA(DFA &dfa, DFA_MIN &min);      // DFA最小化
void prinMinDFA(DFA_MIN min);                   // 输出最小化DFA
int findNextNode(DFA dfa, int num, char trans); // 根据状态转移函数找转移到的下一节点
int AddReList(int i, int j, int p, int q);      // 将一对状态对加入关联链表,添加成功返回在数组中的位置
void labell(DFA &min, int i, int j);            // 根据关联表递归地标记(i,j)的关联链表上的各个状态对在可区分状态表中的对应表项

/************************************************************************* DFA to RG ********************************************/
void transfer_RG(DFA_MIN d); // DFA到RG的转化

/************************************************************************* RG验证 ***********************************************/
bool match(DFA d, string str); // 验证语言是否属于给定的正则表达式

/*********************************************************** 主函数 *********************/
int main(int, char **)
{
    string re;
    NFA nfa;
    DFA dfa;
    DFA_MIN min;
    transfer_RE(re, nfa);
    transfer_NFA(nfa, dfa);
    transfer_DFA(dfa, min);
    // match(min, re);
    transfer_RG(min);
    system("pause");
    return 0;
}

/*********************************************************** RE->NFA *********************/
/*浠庣姸鎬乶1鍒扮姸鎬乶2娣诲姞涓€鏉″姬锛屽姬涓婄殑鍊间负ch*/
void add(NfaState *n1, NfaState *n2, char ch)
{

    n1->input = ch;
    n1->chTrans = n2->index;
}

/*浠庣姸鎬乶1鍒扮姸鎬乶2娣诲姞涓€鏉″姬锛屽姬涓婄殑鍊间负蔚*/
void add(NfaState *n1, NfaState *n2)
{

    n1->epTrans.insert(n2->index);
}

/*鏂板缓涓€涓狽FA锛堝嵆浠嶯FA鐘舵€佹暟缁勪腑鍙栧嚭涓や釜鐘舵€侊級*/
NFA1 creatNFA(int sum)
{

    NFA1 n;

    n.head = &NfaStates[sum];
    n.tail = &NfaStates[sum + 1];

    return n;
}

/*鍦ㄥ瓧绗︿覆s绗琻浣嶅悗闈㈡彃鍏ュ瓧绗h*/
void insert(string &s, int n, char ch)
{

    s += '#'; // 鐘舵€佸姬涓婄殑榛樿鍊?

    for (int i = s.size() - 1; i > n; i--)
    {
        s[i] = s[i - 1];
    }

    s[n] = ch;
}

/*瀵瑰瓧绗︿覆s杩涜棰勫鐞嗭紝鍦ㄧ涓€浣嶆槸鎿嶄綔鏁般€佲€?鈥欐垨鈥?鈥欎笖绗簩浣嶆槸鎿嶄綔鏁版垨鈥?鈥欎箣闂村姞鍏ヨ繛鎺ョ鈥?鈥?*/
void preprocess(string &s)
{ // s鏁扮粍涓哄嚱鏁板弬鏁帮紝闇€瑕佷慨鏀规暟缁勭殑绫诲瀷涓篿nt

    int i = 0, length = s.size();

    while (i < length)
    {
        if ((s[i] == '0' || s[i] == '1') || (s[i] == '*') || (s[i] == ')')) // 鍋氬嚭淇敼鍦╝,z鑼冨洿鍐呯殑骞舵敼涓簊[i]鍜?/1鐨勬垨鍒ゆ柇
        {
            if ((s[i + 1] == '0' || s[i + 1] == '1') || s[i + 1] == '(') // 鍚屼笂
            {

                insert(s, i + 1, '&');
                length++;
            }
        }

        i++;
    }
}

/*涓紑杞悗缂€鏃剁敤鍒扮殑浼樺厛绾ф瘮杈冿紝鍗充负姣忎釜鎿嶄綔绗﹁祴涓€涓潈閲嶏紝閫氳繃鏉冮噸澶у皬姣旇緝浼樺厛绾?*/
int priority(char ch)
{

    if (ch == '*')
    {
        return 3;
    }

    if (ch == '&')
    {
        return 2;
    }

    if (ch == '+') // 鎴栧湪杩欓噷琚敼鎴?鍙?
    {
        return 1;
    }

    if (ch == '(')
    {
        return 0;
    }
}

/*涓紑琛ㄨ揪寮忚浆鍚庣紑琛ㄨ揪寮?*/
string infixToSuffix(string s)
{

    preprocess(s); /*瀵瑰瓧绗︿覆杩涜棰勫鐞?*/

    string str;       /*瑕佽緭鍑虹殑鍚庣紑瀛楃涓?*/
    stack<char> oper; /*杩愮畻绗︽爤*/

    for (int i = 0; i < s.size(); i++)
    {

        if (s[i] == '0' || s[i] == '1') /*濡傛灉鏄搷浣滄暟鐩存帴杈撳嚭*/ // 鍋氬嚭淇敼鍚屼笂
        {
            str += s[i];
        }
        else /*閬囧埌杩愮畻绗︽椂*/
        {

            if (s[i] == '(') /*閬囧埌宸︽嫭鍙峰帇鍏ユ爤涓?*/
            {
                oper.push(s[i]);
            }

            else if (s[i] == ')') /*閬囧埌鍙虫嫭鍙锋椂*/
            {

                char ch = oper.top();
                while (ch != '(') /*灏嗘爤涓厓绱犲嚭鏍堬紝鐩村埌鏍堥《涓哄乏鎷彿*/
                {

                    str += ch;

                    oper.pop();
                    ch = oper.top();
                }

                oper.pop(); /*鏈€鍚庡皢宸︽嫭鍙峰嚭鏍?*/
            }
            else /*閬囧埌鍏朵粬鎿嶄綔绗︽椂*/
            {

                if (!oper.empty()) /*濡傛灉鏍堜笉涓虹┖*/
                {

                    char ch = oper.top();
                    while (priority(ch) >= priority(s[i])) /*寮瑰嚭鏍堜腑浼樺厛绾уぇ浜庣瓑浜庡綋鍓嶈繍绠楃鐨勮繍绠楃*/
                    {

                        str += ch;
                        oper.pop();

                        if (oper.empty()) /*濡傛灉鏍堜负绌哄垯缁撴潫寰幆*/
                        {
                            break;
                        }
                        else
                            ch = oper.top();
                    }

                    oper.push(s[i]); /*鍐嶅皢褰撳墠杩愮畻绗﹀叆鏍?*/
                }

                else /*濡傛灉鏍堜负绌猴紝鐩存帴灏嗚繍绠楃鍏ユ爤*/
                {
                    oper.push(s[i]);
                }
            }
        }
    }

    /*鏈€鍚庡鏋滄爤涓嶄负绌猴紝鍒欏嚭鏍堝苟杈撳嚭鍒板瓧绗︿覆*/
    while (!oper.empty())
    {

        char ch = oper.top();
        oper.pop();

        str += ch;
    }
    return str;
}

/*鍚庣紑琛ㄨ揪寮忚浆nfa*/
NFA1 strToNfa(string s)
{

    stack<NFA1> NfaStack; /*瀹氫箟涓€涓狽FA鏍?*/

    for (int i = 0; i < s.size(); i++) /*璇诲彇鍚庣紑琛ㄨ揪寮忥紝姣忔璇讳竴涓瓧绗?*/
    {

        if (s[i] == '0' || s[i] == '1') /*閬囧埌鎿嶄綔鏁?*/ // 淇敼鍚屼笂
        {

            NFA1 n = creatNFA(nfaStateNum); /*鏂板缓涓€涓狽FA*/
            nfaStateNum += 2;               /*NFA鐘舵€佹€绘暟鍔?*/

            add(n.head, n.tail, s[i]); /*NFA鐨勫ご鎸囧悜灏撅紝寮т笂鐨勫€间负s[i]*/

            NfaStack.push(n); /*灏嗚NFA鍏ユ爤*/
        }

        else if (s[i] == '*') /*閬囧埌闂寘杩愮畻绗?*/
        {

            NFA1 n1 = creatNFA(nfaStateNum); /*鏂板缓涓€涓狽FA*/
            nfaStateNum += 2;                /*NFA鐘舵€佹€绘暟鍔?*/

            NFA1 n2 = NfaStack.top(); /*浠庢爤涓脊鍑轰竴涓狽FA*/
            NfaStack.pop();

            add(n2.tail, n2.head); /*n2鐨勫熬閫氳繃蔚鎸囧悜n1鐨勫ご*/
            add(n2.tail, n1.tail); /*n2鐨勫熬閫氳繃蔚鎸囧悜n1鐨勫熬*/
            add(n1.head, n2.head); /*n1鐨勫ご閫氳繃蔚鎸囧悜n2鐨勫ご*/
            add(n1.head, n1.tail); /*n1鐨勫ご閫氳繃蔚鎸囧悜n1鐨勫熬*/

            NfaStack.push(n1); /*鏈€鍚庡皢鏂扮敓鎴愮殑NFA鍏ユ爤*/
        }

        else if (s[i] == '+') /*閬囧埌鎴栬繍绠楃*/ // 鎴栬繍绠楃闇€瑕佹浛鎹㈢殑鍦版柟涔嬩竴
        {

            NFA1 n1, n2; /*浠庢爤涓脊鍑轰袱涓狽FA锛屾爤椤朵负n2锛屾鏍堥《涓簄1*/
            n2 = NfaStack.top();
            NfaStack.pop();

            n1 = NfaStack.top();
            NfaStack.pop();

            NFA1 n = creatNFA(nfaStateNum); /*鏂板缓涓€涓狽FA*/
            nfaStateNum += 2;               /*NFA鐘舵€佹€绘暟鍔?*/

            add(n.head, n1.head); /*n鐨勫ご閫氳繃蔚鎸囧悜n1鐨勫ご*/
            add(n.head, n2.head); /*n鐨勫ご閫氳繃蔚鎸囧悜n2鐨勫ご*/
            add(n1.tail, n.tail); /*n1鐨勫熬閫氳繃蔚鎸囧悜n鐨勫熬*/
            add(n2.tail, n.tail); /*n2鐨勫熬閫氳繃蔚鎸囧悜n鐨勫熬*/

            NfaStack.push(n); /*鏈€鍚庡皢鏂扮敓鎴愮殑NFA鍏ユ爤*/
        }

        else if (s[i] == '&') /*閬囧埌杩炴帴杩愮畻绗?*/
        {

            NFA1 n1, n2, n; /*瀹氫箟涓€涓柊鐨凬FA n*/

            n2 = NfaStack.top(); /*浠庢爤涓脊鍑轰袱涓狽FA锛屾爤椤朵负n2锛屾鏍堥《涓簄1*/
            NfaStack.pop();

            n1 = NfaStack.top();
            NfaStack.pop();

            add(n1.tail, n2.head); /*n1鐨勫熬閫氳繃蔚鎸囧悜n2鐨勫ご*/

            n.head = n1.head; /*n鐨勫ご涓簄1鐨勫ご*/
            n.tail = n2.tail; /*n鐨勫熬涓簄2鐨勫熬*/

            NfaStack.push(n); /*鏈€鍚庡皢鏂扮敓鎴愮殑NFA鍏ユ爤*/
        }
    }

    return NfaStack.top(); /*最后的栈顶元素即为生成好的NFA*/
}
/*姹備竴涓姸鎬侀泦鐨勎?cloure*/
set<int> epcloure(set<int> s)
{

    stack<int> epStack; /*(姝ゅ鏍堝拰闃熷垪鍧囧彲)*/

    set<int>::iterator it;
    for (it = s.begin(); it != s.end(); it++)
    {
        epStack.push(*it); /*灏嗚鐘舵€侀泦涓殑姣忎竴涓厓绱犻兘鍘嬪叆鏍堜腑*/
    }

    while (!epStack.empty()) /*鍙鏍堜笉涓虹┖*/
    {

        int temp = epStack.top(); /*浠庢爤涓脊鍑轰竴涓厓绱?*/
        epStack.pop();

        set<int>::iterator iter;
        for (iter = NfaStates[temp].epTrans.begin(); iter != NfaStates[temp].epTrans.end(); iter++)
        {
            if (!s.count(*iter))     /*閬嶅巻瀹冮€氳繃蔚鑳借浆鎹㈠埌鐨勭姸鎬侀泦*/
            {                        /*濡傛灉褰撳墠鍏冪礌娌℃湁鍦ㄩ泦鍚堜腑鍑虹幇*/
                s.insert(*iter);     /*鍒欐妸瀹冨姞鍏ラ泦鍚堜腑*/
                epStack.push(*iter); /*鍚屾椂鍘嬪叆鏍堜腑*/
            }
        }
    }

    return s; /*鏈€鍚庣殑s鍗充负蔚-cloure*/
}

/*姹備竴涓姸鎬侀泦s鐨勎?cloure(move(ch))*/
set<int> moveEpCloure(set<int> s, char ch)
{

    set<int> temp;

    set<int>::iterator it;
    for (it = s.begin(); it != s.end(); it++) /*閬嶅巻褰撳墠闆嗗悎s涓殑姣忎釜鍏冪礌*/
    {
        if (NfaStates[*it].input == ch) /*濡傛灉瀵瑰簲杞崲寮т笂鐨勫€间负ch*/
        {
            temp.insert(NfaStates[*it].chTrans); /*鍒欐妸璇ュ姬閫氳繃ch杞崲鍒扮殑鐘舵€佸姞鍏ュ埌闆嗗悎temp涓?*/
        }
    }

    temp = epcloure(temp); /*鏈€鍚庢眰temp鐨勎甸棴鍖?*/
    return temp;
};

/*鍒ゆ柇涓€涓姸鎬佹槸鍚︿负缁堟€?*/
bool IsEnd(NFA1 n, set<int> s)
{

    set<int>::iterator it;
    for (it = s.begin(); it != s.end(); it++) /*閬嶅巻璇ョ姸鎬佹墍鍖呭惈鐨刵fa鐘舵€侀泦*/
    {
        if (*it == n.tail->index) /*濡傛灉鍖呭惈nfa鐨勭粓鎬侊紝鍒欒鐘舵€佷负缁堟€侊紝杩斿洖true*/
        {
            return true;
        }
    }

    return false; /*濡傛灉涓嶅寘鍚紝鍒欎笉鏄粓鎬侊紝杩斿洖false*/
}
void transfer_RE(string &re, NFA &nfa)
{
    cin >> re;
    re = infixToSuffix(re); /*灏嗕腑缂€琛ㄨ揪寮忚浆鎹负鍚庣紑琛ㄨ揪寮?*/

    /***鍒濆鍖栨墍鏈夌殑鏁扮粍***/
    int i, j;
    for (i = 0; i < 128; i++)
    {

        NfaStates[i].index = i;
        NfaStates[i].input = '#';
        NfaStates[i].chTrans = -1;
    }
    NFA1 temp = strToNfa(re);
    nfa.re = re;
    INFA a1, a2;
    a1 = (INFA)malloc(sizeof(INFA));
    a1->num = temp.head->index;
    a1->name = '0';
    nfa.S = a1;
    nfa.stateNum = nfaStateNum;
    set<int>::iterator it;
    for (i = 0; i < nfa.stateNum; i++)
    {

        nfa.graph.push_back(vector<pair<int, char>>{});
        for (char j = '0'; j < '2'; j++)
        {
            // set<int> te = moveEpCloure(NfaStates[i].epTrans, j);
            set<int> te = moveEpCloure(epcloure(set<int>{NfaStates[i].index}), j);
            for (it = te.begin(); it != te.end(); it++)
            {
                nfa.graph[i].push_back(pair<int, char>{*it, j});
            }
        }
    }
    for (i = 0; i < nfa.graph.size(); i++)
    {
        set<int> te = epcloure(set<int>{NfaStates[i].index});
        for (it = te.begin(); it != te.end(); it++)
        {
            if (*it == temp.tail->index)
            {
                INFA l = (INFA)malloc(sizeof(INFA));
                l->num = i;
                nfa.T.push_back(l);
                break;
            }
        }
        /*for (int j = 0; j < nfa.graph[i].size(); j++) {
            if (nfa.graph[i][j].first == temp.tail->index)
            {
                INFA l = (INFA)malloc(sizeof(INFA));
                l->num = i;
                nfa.T.push_back(l);
                break;
            }
        }*/
    }
    return;
};

/*********************************************************** NFA->DFA *********************/
void transfer_NFA(NFA &nfa, DFA &dfa)
{
    IDFA x = (IDFA)malloc(sizeof(IDFA));
    if (x != NULL)
    {
        x->num = nfa.S->num;
        // x->name = 'S';  //就假定它是S吧！
    }
    dfa.S = x;
    int nowSingleState, cnt = 1, _No = 0; // cnt为新状态集计数器,_No用于编号DFA新状态
    queue<set<int>> stateQueue;
    set<int> nowState;
    set<char> letterSet; // 输入符号队列
    set<int> tmp;
    set<set<int>> newState; // DFA的新状态集
    nowState.insert(dfa.S->num);
    newState.insert(nowState); // 将开始状态S加入新状态集中
    stateQueue.push(nowState);
    // 建立符号表
    for (int i = 0; i < nfa.graph.size(); i++) // 遍历nfa的所有边
        for (int j = 0; j < nfa.graph[i].size(); j++)
            letterSet.insert(nfa.graph[i][j].second);
    // 确定新状态
    while (!stateQueue.empty())
    {
        nowState = stateQueue.front();
        stateQueue.pop();
        for (set<char>::iterator thisLetter = letterSet.begin(); thisLetter != letterSet.end(); thisLetter++) // 遍历符号集
        {
            tmp.clear();
            for (set<int>::iterator t = nowState.begin(); t != nowState.end(); t++) // 遍历当前状态下的每一单个状态
            {
                nowSingleState = *t;
                for (int j = 0; j < nfa.graph[nowSingleState].size(); j++) // 遍历从该状态出发的所有边
                {
                    if (nfa.graph[nowSingleState][j].second == *thisLetter)
                        tmp.insert(nfa.graph[nowSingleState][j].first); // 将每一个转移状态加入，以构建新状态集
                }
            }
            if (!tmp.empty())
            {
                newState.insert(tmp); // 将tmp加入到set中，以达到去重目的
            }
        }

        if (cnt != newState.size()) // 若状态集里产生了新状态，进行入队列操作
        {
            int _cnt = 0; //_cnt用于状态集中定位产生的新状态，以方便入队列
            for (set<set<int>>::iterator thisSet = newState.begin(); thisSet != newState.end(); thisSet++)
            {
                if (_cnt == cnt)
                {
                    stateQueue.push(*thisSet); // 入队列
                    cnt++;
                }
                _cnt++;
            }
        }
    }
    // 建立转移图,基本是重复一遍上面的工作
    int state1 = 0, state2 = 0, edgeNum = 0;
    vector<pair<int, char>> _graph; // 临时工
    map<set<int>, int> stateArray;  // 建立新状态对应编号，方便建立转移图
    for (auto u : newState)
        stateArray.insert({u, _No++});
    for (auto v : newState)
        stateQueue.push(v);
    while (!stateQueue.empty())
    {
        nowState = stateQueue.front();
        stateQueue.pop();
        for (set<char>::iterator thisLetter = letterSet.begin(); thisLetter != letterSet.end(); thisLetter++) // 遍历符号集
        {
            tmp.clear();
            for (set<int>::iterator t = nowState.begin(); t != nowState.end(); t++) // 遍历当前状态下的每一单个状态
            {
                nowSingleState = *t;
                for (int j = 0; j < nfa.graph[nowSingleState].size(); j++) // 遍历从该状态出发的所有边
                {
                    if (nfa.graph[nowSingleState][j].second == *thisLetter)
                        tmp.insert(nfa.graph[nowSingleState][j].first); // 将每一个转移状态加入，以构建新状态集
                }
            }
            if (!tmp.empty())
            {
                for (auto c : stateArray)
                    if (Isequal(nowState, c.first))
                    {
                        state1 = c.second; // 当前状态编号
                        break;
                    }
                for (auto d : stateArray)
                    if (Isequal(tmp, d.first))
                    {
                        state2 = d.second; // 转移的下一状态编号
                        break;
                    }
                _graph.push_back(make_pair(state2, *thisLetter));
            }
        }
        dfa.graph.push_back(_graph);
        _graph.clear();
    }

    // 完善DFA
    dfa.stateNum = newState.size(); // 状态数
    for (auto v : stateArray)       // 终态集
    {
        bool temp = false;
        for (auto u : nfa.T)
        {
            if (temp)
                break;
            for (auto w : v.first)
                if (u->num == w)
                {
                    IDFA X = (IDFA)malloc(sizeof(IDFA));
                    if (X != NULL)
                    {
                        X->num = v.second;
                        // X->name = 'S';  //就假定它是S吧！
                    }
                    dfa.T.push_back(X);
                    temp = true;
                }
        }
    }

    bool temp = false;
    int i = 0;
    for (auto v : newState)
    {
        if (temp)
            break;
        for (auto w : v)
        {
            if (w == dfa.S->num)
            {
                dfa.S->num = i;
                temp = true;
            }
        }
        i++;
    }

    return;
}

bool Isequal(set<int> a, set<int> b) // 辅助函数，用于判断两个集合是否相等
{
    if (a.size() != b.size())
        return 0;
    else
    {
        set<int>::iterator u = a.begin();
        set<int>::iterator v = b.begin();
        for (int i = 0; i < a.size(); i++)
        {
            if (*u != *v)
                return 0;
            u++;
            v++;
        }
    }
    return 1;
}

/*********************************************************** DFA最小化 *********************/
typedef struct reNode
{
    int fir;
    int sec;
    struct reNode *next;
} *relate; // 关联链表

vector<relate> regraph; // 一组关联链表

void transfer_DFA(DFA &dfa, DFA_MIN &min)
{
    // 初始化DFA所有结点
    for (int i = 0; i < dfa.stateNum; i++)
    {
        DFANode q;
        q.num = i;
    }

    // 判断是否是终止状态的数组
    vector<int> ter;
    int i = 0;
    for (int k = 0; k < dfa.T.size(); k++)
    {
        while (i != dfa.T[k]->num)
        {
            ter.push_back(-1);
            i++;
        }
        ter.push_back(i);
        i++;
    }
    for (; i < dfa.stateNum; i++)
    {
        ter.push_back(-1);
        i++;
    }

    min = dfa; // 把dfa的值赋给min

    /*************第一步：可区分状态表的建立*********************/
    // 初始化可区分状态表的所有状态为不可区分
    for (int i = 1; i < dfa.stateNum; i++)
    {
        vector<bool> a;
        a.push_back(false);
        min.flag.push_back(a);
        for (int j = 1; j < i; j++)
        {
            bool b = false;
            min.flag[i - 1].push_back(b);
        }
    }

    // 任意终止状态与非终止状态一定可区分
    for (int i = 1; i < dfa.stateNum; i++)
    {
        if (ter[i] != -1)
        { // 终止状态
            for (int j = 0; j < i; j++)
            {
                if (ter[j] == -1)
                { // 非终止状态
                    min.flag[i - 1][j] = true;
                }
            }
        }
        else if (ter[i] == -1)
        { // 非终止状态
            for (int j = 0; j < i; j++)
            {
                if (ter[j] != -1)
                { // 终止状态
                    min.flag[i - 1][j] = true;
                }
            }
        }
    }
    // 处理非终止状态与非终止状态，终止状态与终止状态
    for (int i = 1; i < dfa.stateNum; i++)
    {
        for (int j = 0; j < i; j++)
        {
            //(i,j)之间没有标记，则查看关系图求(qi,0)=p,(qj,0)=q；(qi,1)=h,(qj,1)=l
            if (!min.flag[i - 1][j])
            {
                // 0转移
                int p = findNextNode(dfa, i, '0');
                int q = findNextNode(dfa, j, '0');
                // 如果p,q都存在且不相等
                if ((p != -1) && (q != -1) && (p != q))
                {
                    // 使前者比后者编号大
                    if (p < q)
                    {
                        int temp = p;
                        p = q;
                        q = temp;
                    }
                    // 使(p,q)!=(i,j)
                    if (p != i && q != j)
                    {
                        // 如果p,q都存在且(p,q)之间没有标记时，创建关联链表
                        if (!min.flag[p - 1][q])
                        {
                            AddReList(i, j, p, q);
                        }
                        // 如果p,q都存在且(p,q)之间有标记时，则标记(i,j)及其关联表上的状态对
                        else
                        {
                            labell(min, i, j);
                        }
                    }
                }
                // p,q不存在则不做处理

                // 1转移
                int h = findNextNode(dfa, i, '1');
                int l = findNextNode(dfa, j, '1');
                // 如果h,l都存在且不相等
                if ((h != -1) && (l != -1) && (h != l))
                {
                    // 使前者比后者编号大
                    if (h < l)
                    {
                        int temp = h;
                        h = l;
                        l = temp;
                    }
                    // 使(h,l)!=(i,j)
                    if (h == i && l == j)
                    {
                        break;
                    }
                    else
                    {
                        // 如果h,l都存在且(h,l)之间没有标记时，创建关联链表
                        if (!min.flag[h - 1][l])
                        {
                            AddReList(i, j, h, l);
                        }
                        // 如果h,l都存在且(h,l)之间有标记时，则标记(i,j)及其关联表上的状态对
                        else
                        {
                            labell(min, i, j);
                        }
                    }
                }
                // h,l不存在则不做处理

                // 0转移和1转移二者各有一个不存在，则二者可区分
                if (((h != -1) && (l == -1)) || ((h == -1) && (l != -1)) || ((p != -1) && (q == -1)) || ((p == -1) && (q != -1)))
                {
                    labell(min, i, j);
                }
            }
            //(i,j)之间已经有标记则不做处理
        }
    }

    /*************第二步：更新MINDFA的结点:创建最小化DFA的状态的二维数组（同一行的是不可区分状态）*********************/
    // q0结点,第一行放入起始结点并查找不可区分结点
    vector<int> a;
    a.push_back(min.S->num);
    if (min.S->num == dfa.stateNum)
    {
        for (int i = 0; i < dfa.stateNum - 1; i++)
        {
            // q0的状态判断，如果发现不可区分结点，则将该结点放入同一行
            if (min.flag[min.S->num][i] == false)
            {
                a.push_back(i);
            }
        }
        // 将这一行插入二维数组中
        min.node.push_back(a);
    }
    else if (min.S->num == 0)
    {
        for (int i = 1; i < dfa.stateNum; i++)
        {
            // q0的状态判断，如果发现不可区分结点，则将该结点放入同一行
            if (min.flag[i - 1][min.S->num] == false)
            {
                a.push_back(i);
            }
        }
        // 将这一行插入二维数组中
        min.node.push_back(a);
    }
    else
    {
        // flag的行
        for (int i = 0; i < min.S->num; i++)
        {
            // q0的状态判断，如果发现不可区分结点，则将该结点放入同一行
            if (min.flag[min.S->num - 1][i] == false)
            {
                a.push_back(i);
            }
        }
        // flag的列
        for (int i = min.S->num; i < dfa.stateNum - 1; i++)
        {
            // q0的状态判断，如果发现不可区分结点，则将该结点放入同一行
            if (min.flag[i][min.S->num] == false)
            {
                a.push_back(i);
            }
        }
        // 将这一行插入二维数组中
        min.node.push_back(a);
    }

    // 除q0结点外的结点（倒序遍历）
    for (int i = dfa.stateNum - 1; i >= 0; i--)
    {
        if (i != min.S->num)
        {
            // 检验该结点是否已经与之前的结点是不可区分状态
            for (int m = 0; m < min.node.size(); m++)
            {
                for (int n = 0; n < min.node[m].size(); n++)
                {
                    // 如果是的话，就将直接更新MINDFA的结点表
                    if (i == min.node[m][n])
                    {
                        // 遍历该结点的可区分状态表，更新MINDFA的结点表
                        for (int j = 0; j < i; j++)
                        {
                            // 如果发现不可区分结点，则将则将该结点放入同一行
                            if (min.flag[i - 1][j] == false)
                            {
                                min.node[m].push_back(j);
                            }
                        }
                        goto TAG;
                    }
                }
            }
            // 如果与之前的都可区分，就清空数组a，并将当前结点放入这一行的数组中
            a.clear();
            a.push_back(i);
            // 按行遍历状态表
            for (int j = 0; j < i; j++)
            {
                // 如果发现不可区分结点，则将则将该结点放入同一行
                if (min.flag[i - 1][j] == false)
                {
                    a.push_back(j);
                }
            }
            // 将这一行插入二维数组中
            min.node.push_back(a);
        TAG:
            int ceshi{0};
        }
    }

    // 规范node数组
    int help = min.node.size();
    while (help--)
    {
        sort(min.node[help].begin(), min.node[help].end());
        min.node[help].erase(unique(min.node[help].begin(), min.node[help].end()), min.node[help].end()); // 去掉重复元素
    }

    // 反转node结点数组（除了q0结点）
    reverse(min.node.begin() + 1, min.node.end());

    // 终止状态的数组
    vector<int> terr;

    // 更新DFA状态关系图,最小DFA的终止节点
    min.graph.clear();
    min.T.clear();
    int k = 0;
    for (int i = 0; i < min.node.size(); i++)
    {
        k = min.node[i][0];
        // 更新最小DFA的终止节点
        terr.push_back(-1);
        if (ter[k] != -1)
        {
            terr.pop_back();
            terr.push_back(k);
        }
        vector<pair<int, char>> a;
        // 找当前结点的下一节点
        int temp0 = findNextNode(dfa, min.node[i][0], '0');
        if (temp0 == -1)
        {
            a.push_back(make_pair(-1, '0'));
        }
        else
        {
            for (int l = 0; l < min.node.size(); l++)
            {
                for (int p = 0; p < min.node[l].size(); p++)
                {
                    if (temp0 == min.node[l][p])
                    {
                        a.push_back(make_pair(l, '0'));
                    }
                }
            }
        }

        int temp1 = findNextNode(dfa, min.node[i][0], '1');
        if (temp1 == -1)
        {
            a.push_back(make_pair(-1, '1'));
        }
        else
        {
            for (int l = 0; l < min.node.size(); l++)
            {
                for (int p = 0; p < min.node[l].size(); p++)
                {
                    if (temp1 == min.node[l][p])
                    {
                        a.push_back(make_pair(l, '1'));
                    }
                }
            }
        }
        min.graph.push_back(a);
    }

    // 更新最小DFA的状态数
    min.stateNum = min.node.size();

    // 更新最小DFA的S集合
    int record = min.S->num;
    for (int l = 0; l < min.node.size(); l++)
    {
        for (int p = 0; p < min.node[l].size(); p++)
        {
            if (record == min.node[l][p])
            {
                min.S->num = l;
            }
        }
    }

    // 更新最小DFA的T集合
    for (int i = 0; i < min.stateNum; i++)
    {
        if (terr[i] != -1)
        {
            IDFA X = (IDFA)malloc(sizeof(IDFA));
            if (X != NULL)
            {
                X->num = i;
            }
            min.T.push_back(X);
        }
    }

    /*************第三步：输出最小化DFA*********************/
    // 输出最小化DFA：第一行输出6个空格，输出0 1，0、1中间用' '隔开。设n为最小DFA的状态数，以下n+1行，每行输出为：
    cout << "      " << 0 << " " << 1 << endl;
    // 第一行状态为起始状态
    cout << "(s)q0 ";
    if (min.graph[0][0].first != -1)
    {
        cout << "q" << min.graph[0][0].first;
    }
    else
    {
        cout << "N";
    }
    if (min.graph[0][1].first != -1)
    {
        cout << " " << "q" << min.graph[0][1].first;
    }
    else
    {
        cout << " " << "N";
    }
    cout << endl;

    // 之后状态按照q1, q2，q3......的顺序命名，终止状态前用(e)标识，每行行末为换行符\n，状态与转换之间使用制表符\t
    k = 0;
    for (int i = 1; i < min.stateNum; i++)
    {
        k = min.node[i][0];
        if (terr[i] != -1)
        { // 终止状态
            cout << "(e)q" << i << " ";
            if (min.graph[i][0].first != -1)
            {
                cout << "q" << min.graph[i][0].first;
            }
            else
            {
                cout << "N";
            }
            if (min.graph[i][1].first != -1)
            {
                cout << " " << "q" << min.graph[i][1].first;
            }
            else
            {
                cout << " " << "N";
            }
            cout << endl;
        }
        else
        {
            cout << "q" << i;
            if (min.graph[i][0].first != -1)
            {
                cout << " " << "q" << min.graph[i][0].first;
            }
            else
            {
                cout << "N";
            }
            if (min.graph[i][1].first != -1)
            {
                cout << " " << "q" << min.graph[i][1].first;
            }
            else
            {
                cout << " " << "N";
            }
            cout << endl;
        }
    }
}

// 根据状态转移函数找转移到的下一节点
int findNextNode(DFA dfa, int num, char trans)
{
    if ((dfa.graph[num].empty()))
        return -1; // 如果该结点没有出边，则返回-1
    // 有出边的话，则寻找对于num节点，输入为trans时指向的下一节点
    for (int k = 0; k < dfa.graph[num].size(); k++)
    {
        if (dfa.graph[num][k].second == trans)
        {
            return dfa.graph[num][k].first; // 找到了则返回该结点的编号
        }
    }
    return -1; // 遍历之后仍未找到则返回-1
}

// 将一对状态对加入关联链表
int AddReList(int i, int j, int p, int q)
{
    // 找(p,q)是否在当前关联表中
    for (int k = 0; k < regraph.size(); k++)
    {
        // 根据头指针找到首结点
        relate temp = regraph[k]->next;
        //(p,q)在当前关联表中
        if (temp->fir == p && temp->sec == q)
        {
            // 找尾结点
            while (temp->next)
            {
                temp = temp->next;
            }
            relate newNode = new reNode; // 创建新结点（i,j）
            newNode->fir = i;
            newNode->sec = j;
            newNode->next = NULL;
            temp->next = newNode; //(p,q)指向（i,j）
            return k;             // 返回头指针在数组中的位置
        }
    }
    // 不在当前关联表内就创建一个新的头指针，(p,q)为首结点
    relate head = new reNode;    // 创建一个指向状态对的指针
    regraph.push_back(head);     // 将该指针加入关联链表数组
    relate pqNode = new reNode;  // 创建新结点 (p,q)
    relate newNode = new reNode; // 创建新结点（i,j）

    // 初始化结点（i,j）
    newNode->fir = i;
    newNode->sec = j;
    newNode->next = NULL;

    // 初始化结点 (p,q)
    head->next = pqNode;
    pqNode->fir = p;
    pqNode->sec = q;
    pqNode->next = newNode;    //(p,q)指向（i,j）
    return regraph.size() - 1; // 返回头指针在数组中的位置
}

// 根据关联表递归地标记(i,j)的关联链表上的各个状态对在可区分状态表中的对应表项
void labell(DFA &min, int i, int j)
{
    min.flag[i - 1][j] = true;

    // 如果(i,j)在关联表内，递归标记
    relate temp = NULL;
    // 根据头指针找到首结点
    for (int k = 0; k < regraph.size(); k++)
    {
        temp = regraph[k]->next;
        if (temp->fir == i && temp->sec == j)
        {
            break;
        }
    }
    if (temp != NULL)
    {
        while (temp->next)
        { // 递归地标记(i,j)的关联链表上的各个状态对在可区分状态表中的对应表项
            int a = temp->next->fir;
            int b = temp->next->sec;
            min.flag[a - 1][b] = true;
            temp = temp->next;
        }
    }
}

/*********************************************************** DFA转RG *********************/
#define MAX 128

void transfer_RG(DFA_MIN d)
{

    cout << endl;
    bool t0, t1;
    for (int i = 0; i < d.stateNum; i++)
    {
        t0 = t1 = false;
        if (d.graph[i][0].first != -1 && d.graph[i][0].first != d.T[0]->num)
        {
            cout << "q" << i << "->0q" << d.graph[i][0].first << endl;
        }
        else if (d.graph[i][0].first == d.T[0]->num)
        {
            if (d.graph[d.T[0]->num][0].first != -1 || d.graph[d.T[0]->num][1].first != -1)
                cout << "q" << i << "->0q" << d.graph[i][0].first << endl;
            t0 = true;
        }
        else
            ;
        if (d.graph[i][1].first != -1 && d.graph[i][1].first != d.T[0]->num)
            cout << "q" << i << "->1q" << d.graph[i][1].first << endl;
        else if (d.graph[i][1].first == d.T[0]->num)
        {
            if (d.graph[d.T[0]->num][0].first != -1 || d.graph[d.T[0]->num][1].first != -1)
                cout << "q" << i << "->1q" << d.graph[i][1].first << endl;
            t1 = true;
        }
        else
            ;
        if (t0)
            cout << "q" << i << "->0" << endl;
        if (t1)
            cout << "q" << i << "->1" << endl;
    }
}

/*********************************************************** RG验证 *********************/
bool match(DFA d, string str)
{
    int length = str.size();
    int i = 0;
    int index;
    index = d.S->num; // index为开始状态的编号
    while (i < length)
    {
        bool flag = false;
        for (int j = 0; j < d.graph[index].size(); j++)
        {
            // 判断状态index是否与这样的边相连，边上接受的输入为str[i]
            if (d.graph[index][j].second == str[i])
            {
                index = d.graph[index][j].first;
                // 如果index存在这样的边，那么进行状态的转移
                flag = true;
                break;
            }
        }
        if (!flag)
            return false;
        i++;
    }
    // 判断当前所处的状态是否为接收状态
    for (int i = 0; i < d.T.size(); i++)
        if (d.T[i]->num == index)
            return true;
}