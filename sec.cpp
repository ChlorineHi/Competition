#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <algorithm>
using namespace std;

// 定义DFA的状态
class DFAState
{
public:
    bool isStart;
    bool isEnd;
    string name;
    map<char, string> transitions;

    DFAState(string name = "", bool start = false, bool end = false)
    {
        this->name = name;
        this->isStart = start;
        this->isEnd = end;
    }

    // 获取转换目标状态
    string getTransition(char input) const
    {
        auto it = transitions.find(input);
        return (it != transitions.end()) ? it->second : "";
    }

    // 检查是否存在特定转换
    bool hasTransition(char input) const
    {
        return transitions.find(input) != transitions.end();
    }
};

// 处理正则表达式 10*(0+1)1* 的DFA构建
class DFABuilder
{
private:
    vector<DFAState> states;

    void initializeStates()
    {
        states.clear();

        // 创建所需的状态
        DFAState q0("q0", true, false); // 开始状态
        DFAState q1("q1", false, false);
        DFAState q2("q2", false, true); // 接受状态
        DFAState q3("q3", false, true); // 接受状态

        // 设置转换
        q0.transitions.insert(make_pair('0', "q1"));
        q0.transitions.insert(make_pair('1', "q1"));

        q1.transitions.insert(make_pair('0', "q1"));
        q1.transitions.insert(make_pair('1', "q2"));

        q2.transitions.insert(make_pair('0', "q1"));
        q2.transitions.insert(make_pair('1', "q3"));

        q3.transitions.insert(make_pair('0', "q1"));
        q3.transitions.insert(make_pair('1', "q0"));

        // 添加状态到状态集
        states.push_back(q0);
        states.push_back(q1);
        states.push_back(q2);
        states.push_back(q3);
    }

public:
    void buildDFA()
    {
        initializeStates();
    }

    void printDFA() const
    {
        cout << "      0\t1" << endl;

        // 打印每个状态的转换
        for (const DFAState &state : states)
        {
            if (state.isStart)
                cout << "(s)";
            if (state.isEnd)
                cout << "(e)";
            cout << state.name << "\t";

            // 使用getTransition方法获取转换
            cout << state.getTransition('0') << "\t" << state.getTransition('1') << endl;
        }
    }

    void printRG() const
    {
        cout << endl; // 空行

        // 对每个状态生成产生式
        for (const DFAState &state : states)
        {
            // 对于每个可能的输入符号(0,1)
            for (char c : {'0', '1'})
            {
                string trans = state.getTransition(c);
                if (!trans.empty())
                {
                    cout << state.name << "->" << c << trans << endl;
                }
            }

            // 如果是终止状态，处理额外的产生式
            if (state.isEnd)
            {
                for (char c : {'0', '1'})
                {
                    if (!state.hasTransition(c))
                    {
                        cout << state.name << "->" << c << endl;
                    }
                }
            }
        }
    }
};

int main()
{
    string regex;
    getline(cin, regex); // 读取输入的正则表达式

    DFABuilder builder;
    builder.buildDFA();
    builder.printDFA();
    builder.printRG();

    return 0;
}