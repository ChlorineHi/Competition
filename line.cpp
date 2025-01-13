#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <string>

using namespace std;

// 定义一个状态
struct State
{
    string name;
    bool isEnd;                                      // 是否为终止状态
    unordered_map<char, vector<string>> transitions; // 转移关系
};

// 全局变量
vector<State> nfa;                        // NFA 状态
vector<State> dfa;                        // DFA 状态
unordered_map<string, vector<string>> rg; // 正则文法

// 将正则表达式转换为 ε-NFA
void regexToNFA(const string &regex)
{
    // 这里可以实现更复杂的正则表达式解析
    // 暂时只支持简单的正则表达式示例
    State startState;
    startState.name = "q0";
    startState.isEnd = false;

    State endState;
    endState.name = "q1";
    endState.isEnd = true;

    // 示例仅处理 a*b 形式的正则表达式
    startState.transitions['0'].push_back(endState.name);
    startState.transitions['1'].push_back(endState.name);
    nfa.push_back(startState);
    nfa.push_back(endState);
}

// 将 ε-NFA 转换为 NFA
void eNFAtoNFA()
{
    // 简化实现: 此处省略 ε-NFA 到 NFA 的处理
}

// 将 NFA 转换为 DFA
void nfaToDFA()
{
    // 使用子集构造法转换 NFA 为 DFA
    queue<vector<string>> statesQueue;
    set<string> visited;

    vector<string> startState = {nfa[0].name}; // 初始状态
    statesQueue.push(startState);
    visited.insert(nfa[0].name);

    while (!statesQueue.empty())
    {
        vector<string> currentStates = statesQueue.front();
        statesQueue.pop();

        string dfaStateName = "q" + to_string(dfa.size());
        State dfaState;
        dfaState.name = dfaStateName;
        dfaState.isEnd = false;

        // 遍历输入字符 0 和 1
        for (char input : {'0', '1'})
        {
            vector<string> nextStates;
            for (const string &s : currentStates)
            {
                for (const auto &target : nfa)
                {
                    if (target.name == s)
                    {
                        // 添加到下一个状态
                        if (target.transitions.find(input) != target.transitions.end())
                        {
                            nextStates.insert(nextStates.end(), target.transitions.at(input).begin(),
                                              target.transitions.at(input).end());
                        }
                    }
                }
            }

            // 如果有下一个状态，则记录转移
            if (!nextStates.empty())
            {
                // 移除重复状态
                sort(nextStates.begin(), nextStates.end());
                nextStates.erase(unique(nextStates.begin(), nextStates.end()), nextStates.end());

                dfaState.transitions[input] = nextStates;

                // 检查是否已经访问过
                string nextStateStr = to_string(dfa.size() + 1);
                if (visited.find(nextStateStr) == visited.end())
                {
                    statesQueue.push(nextStates);
                    visited.insert(nextStateStr);
                }
            }
        }

        dfa.push_back(dfaState);
    }
}

// 最小化 DFA（未实现）
void minimizeDFA()
{
    // 用于实现 DFA 最小化逻辑
    // 这里可以插入最小化代码
}

// 生成正则文法
void generateRG()
{
    // 遍历 DFA 状态生成 RG
    for (const State &state : dfa)
    {
        string variable = state.name;
        for (const auto &transition : state.transitions)
        {
            for (const string &nextState : transition.second)
            {
                rg[variable].push_back(variable + "->" + transition.first + nextState);
            }
        }
    }
}

// 打印最小化 DFA
void printDFA()
{
    cout << "0\t1\n";
    for (const State &state : dfa)
    {
        cout << state.name;
        for (char input : {'0', '1'})
        {
            if (state.transitions.find(input) != state.transitions.end())
            {
                for (const string &nextState : state.transitions.at(input))
                {
                    cout << "\t" << nextState;
                }
            }
            else
            {
                cout << "\t";
            }
        }
        cout << endl;
    }
}

// 打印 RG
void printRG()
{
    for (const auto &rule : rg)
    {
        for (const string &productions : rule.second)
        {
            cout << productions << endl;
        }
    }
}

int main()
{
    string regex;
    cout << "请输入正则表达式: ";
    cin >> regex;

    // 流程开始
    regexToNFA(regex);
    eNFAtoNFA();
    nfaToDFA();
    minimizeDFA();
    generateRG();

    cout << "最小化 DFA:\n";
    printDFA();

    cout << "正则文法:\n";
    printRG();

    return 0;
}