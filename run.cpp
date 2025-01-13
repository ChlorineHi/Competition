#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
using namespace std;

// 状态类
class State
{
public:
    string name;
    bool isStart;
    bool isEnd;
    map<char, set<State *>> transitions; // 转移函数

    State(string n, bool start = false, bool end = false)
        : name(n), isStart(start), isEnd(end) {}
};

// NFA类
class NFA
{
public:
    vector<State *> states;
    State *startState;
    set<State *> endStates;

    void addState(State *state)
    {
        states.push_back(state);
        if (state->isStart)
            startState = state;
        if (state->isEnd)
            endStates.insert(state);
    }

    void addTransition(State *from, char symbol, State *to)
    {
        from->transitions[symbol].insert(to);
    }
};

// DFA类
class DFA
{
public:
    vector<State *> states;
    State *startState;
    set<State *> endStates;

    void addState(State *state)
    {
        states.push_back(state);
        if (state->isStart)
            startState = state;
        if (state->isEnd)
            endStates.insert(state);
    }

    void addTransition(State *from, char symbol, State *to)
    {
        from->transitions[symbol].clear(); // DFA每个输入只能有一个转移
        from->transitions[symbol].insert(to);
    }
};

// 正则表达式处理类
class RegexProcessor
{
private:
    // 构建基本NFA（单个字符）
    NFA *buildBasicNFA(char c)
    {
        NFA *nfa = new NFA();

        State *start = new State("q0", true, false);
        State *end = new State("q1", false, true);

        nfa->addState(start);
        nfa->addState(end);
        nfa->addTransition(start, c, end);

        return nfa;
    }

    // 连接操作
    NFA *concat(NFA *n1, NFA *n2)
    {
        // 将n2的开始状态的转移都加到n1的结束状态上
        for (State *endState : n1->endStates)
        {
            endState->isEnd = false;
            for (auto &transition : n2->startState->transitions)
            {
                char symbol = transition.first;
                for (State *target : transition.second)
                {
                    n1->addTransition(endState, symbol, target);
                }
            }
        }

        // 合并状态集
        for (State *state : n2->states)
        {
            if (state != n2->startState)
            {
                n1->addState(state);
            }
        }

        return n1;
    }

    // 选择操作 (|)
    NFA *alternate(NFA *n1, NFA *n2)
    {
        NFA *nfa = new NFA();

        State *newStart = new State("q0", true, false);
        State *newEnd = new State("qf", false, true);

        nfa->addState(newStart);

        // 添加ε转移到两个NFA的开始状态
        nfa->addTransition(newStart, 'ε', n1->startState);
        nfa->addTransition(newStart, 'ε', n2->startState);

        // 合并两个NFA的状态
        for (State *state : n1->states)
        {
            nfa->addState(state);
        }
        for (State *state : n2->states)
        {
            nfa->addState(state);
        }

        // 从所有终止状态添加ε转移到新的终止状态
        for (State *endState : n1->endStates)
        {
            endState->isEnd = false;
            nfa->addTransition(endState, 'ε', newEnd);
        }
        for (State *endState : n2->endStates)
        {
            endState->isEnd = false;
            nfa->addTransition(endState, 'ε', newEnd);
        }

        nfa->addState(newEnd);
        return nfa;
    }

    // 获取epsilon闭包
    set<State *> epsilonClosure(set<State *> &states)
    {
        set<State *> closure = states;
        queue<State *> queue;

        // 将初始状态集加入队列
        for (State *state : states)
        {
            queue.push(state);
        }

        // 广度优先搜索所有可达的epsilon转移
        while (!queue.empty())
        {
            State *current = queue.front();
            queue.pop();

            if (current->transitions.count('ε'))
            {
                for (State *next : current->transitions['ε'])
                {
                    if (closure.insert(next).second)
                    {
                        queue.push(next);
                    }
                }
            }
        }

        return closure;
    }

    // 获取状态集通过某个输入符号可达的所有状态
    set<State *> move(const set<State *> &states, char symbol)
    {
        set<State *> result;

        for (State *state : states)
        {
            if (state->transitions.count(symbol))
            {
                for (State *next : state->transitions[symbol])
                {
                    result.insert(next);
                }
            }
        }

        return result;
    }

    // 获取一个状态通过某个输入到达的状态
    State *getTransitionState(State *state, char symbol)
    {
        if (state->transitions.count(symbol) && !state->transitions[symbol].empty())
        {
            return *state->transitions[symbol].begin();
        }
        return nullptr;
    }

    // 获取所有能通过某个输入��达指定状态的状态
    set<State *> getInverseTransitions(DFA *dfa, State *target, char symbol)
    {
        set<State *> result;
        for (State *state : dfa->states)
        {
            State *trans = getTransitionState(state, symbol);
            if (trans == target)
            {
                result.insert(state);
            }
        }
        return result;
    }

public:
    // 将正则表达式转换为NFA
    NFA *regexToNFA(string regex)
    {
        vector<NFA *> stack;

        for (size_t i = 0; i < regex.length(); i++)
        {
            char c = regex[i];

            if (c == '(' || c == ')' || c == '+')
                continue;

            if (isalnum(c))
            {
                stack.push_back(buildBasicNFA(c));
            }

            // 处理连接
            if (stack.size() >= 2)
            {
                NFA *n2 = stack.back();
                stack.pop_back();
                NFA *n1 = stack.back();
                stack.pop_back();
                stack.push_back(concat(n1, n2));
            }
        }

        // 如果栈中还有多个NFA，用选择操作合并它们
        while (stack.size() > 1)
        {
            NFA *n2 = stack.back();
            stack.pop_back();
            NFA *n1 = stack.back();
            stack.pop_back();
            stack.push_back(alternate(n1, n2));
        }

        return stack.back();
    }

    // NFA转换为DFA
    DFA *NFAtoDFA(NFA *nfa)
    {
        DFA *dfa = new DFA();
        map<set<State *>, State *> dfaStates; // NFA状态集到DFA状态的映射
        queue<set<State *>> unprocessed;      // 待处理的NFA状态集

        // 计算初始状态的epsilon闭包
        set<State *> initial = {nfa->startState};
        set<State *> initialClosure = epsilonClosure(initial);

        // 创建DFA的初始状态
        State *dfaStart = new State("q0", true);
        dfa->addState(dfaStart);
        dfaStates[initialClosure] = dfaStart;
        unprocessed.push(initialClosure);

        // 检查初始状态是否为终止状态
        for (State *s : initialClosure)
        {
            if (s->isEnd)
            {
                dfaStart->isEnd = true;
                break;
            }
        }

        int stateCounter = 1; // 用于生成新状态名称

        // 处理所有可能的状态转移
        while (!unprocessed.empty())
        {
            set<State *> currentSet = unprocessed.front();
            unprocessed.pop();
            State *currentDFAState = dfaStates[currentSet];

            // 对每个输入符号(0和1)处理转移
            for (char symbol : {'0', '1'})
            {
                // 计算转移后的状态集
                set<State *> nextSet = move(currentSet, symbol);
                set<State *> nextClosure = epsilonClosure(nextSet);

                if (nextClosure.empty())
                    continue;

                // 如果是新的状态集，创建新的DFA状态
                if (dfaStates.find(nextClosure) == dfaStates.end())
                {
                    string newName = "q" + to_string(stateCounter++);
                    State *newState = new State(newName);

                    // 检查是否应该是终止状态
                    for (State *s : nextClosure)
                    {
                        if (s->isEnd)
                        {
                            newState->isEnd = true;
                            break;
                        }
                    }

                    dfa->addState(newState);
                    dfaStates[nextClosure] = newState;
                    unprocessed.push(nextClosure);
                }

                // 添加转移
                dfa->addTransition(currentDFAState, symbol, dfaStates[nextClosure]);
            }
        }

        return dfa;
    }

    DFA *minimizeDFA(DFA *dfa)
    {
        // 1. 将状态分成两组：终止状态和非终止状态
        vector<set<State *>> partitions;
        set<State *> acceptStates, nonAcceptStates;

        for (State *state : dfa->states)
        {
            if (state->isEnd)
            {
                acceptStates.insert(state);
            }
            else
            {
                nonAcceptStates.insert(state);
            }
        }

        if (!acceptStates.empty())
            partitions.push_back(acceptStates);
        if (!nonAcceptStates.empty())
            partitions.push_back(nonAcceptStates);

        // 2. 细化分区
        bool changed;
        do
        {
            changed = false;
            vector<set<State *>> newPartitions;

            // 对每个分区进行处理
            for (const set<State *> &partition : partitions)
            {
                if (partition.size() <= 1)
                {
                    newPartitions.push_back(partition);
                    continue;
                }

                // 尝试分割当前分区
                map<pair<int, int>, set<State *>> splits;
                for (State *state : partition)
                {
                    pair<int, int> key = {-1, -1};

                    // 对于每个输入符号
                    for (char symbol : {'0', '1'})
                    {
                        State *next = getTransitionState(state, symbol);
                        if (next)
                        {
                            // 找出next所在的分区
                            for (size_t i = 0; i < partitions.size(); i++)
                            {
                                if (partitions[i].count(next))
                                {
                                    if (symbol == '0')
                                        key.first = i;
                                    else
                                        key.second = i;
                                    break;
                                }
                            }
                        }
                    }
                    splits[key].insert(state);
                }

                // 如果产生了多个子分区，则添加到新分区中
                if (splits.size() > 1)
                {
                    changed = true;
                    for (const auto &split : splits)
                    {
                        newPartitions.push_back(split.second);
                    }
                }
                else
                {
                    newPartitions.push_back(partition);
                }
            }

            partitions = newPartitions;
        } while (changed);

        // 3. 构建最小化DFA
        DFA *minDFA = new DFA();
        map<set<State *>, State *> partitionToState;

        // 为每个分区创建新状态
        for (const set<State *> &partition : partitions)
        {
            State *representative = *partition.begin();
            string newName = "q" + to_string(partitionToState.size());
            State *newState = new State(newName);

            // 设置起始状态和终止状态
            if (partition.count(dfa->startState))
            {
                newState->isStart = true;
            }
            if (representative->isEnd)
            {
                newState->isEnd = true;
            }

            minDFA->addState(newState);
            partitionToState[partition] = newState;
        }

        // 添加转移
        for (const auto &partition : partitions)
        {
            State *representative = *partition.begin();
            State *fromState = partitionToState[partition];

            for (char symbol : {'0', '1'})
            {
                State *next = getTransitionState(representative, symbol);
                if (next)
                {
                    // 找到next所在的分区
                    for (const auto &targetPartition : partitions)
                    {
                        if (targetPartition.count(next))
                        {
                            minDFA->addTransition(fromState, symbol, partitionToState[targetPartition]);
                            break;
                        }
                    }
                }
            }
        }

        return minDFA;
    }

    // 输出最小化DFA
    void printMinDFA(DFA *dfa)
    {
        cout << "      0 1" << endl;

        // 首先输出起始状态
        for (State *state : dfa->states)
        {
            if (state->isStart)
            {
                cout << "(s)";
                if (state->isEnd)
                    cout << "(e)";
                cout << state->name << " ";

                // ���出转移
                for (char symbol : {'0', '1'})
                {
                    State *next = getTransitionState(state, symbol);
                    cout << (next ? next->name : "N") << " ";
                }
                cout << endl;
                break;
            }
        }

        // 输出其他状态
        for (State *state : dfa->states)
        {
            if (!state->isStart)
            {
                if (state->isEnd)
                    cout << "(e)";
                cout << state->name << " ";

                // 输出转移
                for (char symbol : {'0', '1'})
                {
                    State *next = getTransitionState(state, symbol);
                    cout << (next ? next->name : "N") << " ";
                }
                cout << endl;
            }
        }
    }

    // 输出正则文法
    void printRegularGrammar(DFA *dfa)
    {
        cout << endl;
        for (State *state : dfa->states)
        {
            if (!state->isStart)
                continue;

            // 处理起始状态的产生式
            for (char symbol : {'0', '1'})
            {
                State *next = getTransitionState(state, symbol);
                if (next)
                {
                    if (next->isEnd)
                    {
                        cout << state->name << "->" << symbol << endl;
                    }
                    else
                    {
                        cout << state->name << "->" << symbol << next->name << endl;
                    }
                }
            }
        }
    }
};

int main()
{
    string regex;
    getline(cin, regex);

    RegexProcessor processor;
    NFA *nfa = processor.regexToNFA(regex);
    DFA *dfa = processor.NFAtoDFA(nfa);
    DFA *minDFA = processor.minimizeDFA(dfa);

    processor.printMinDFA(minDFA);
    processor.printRegularGrammar(minDFA);

    return 0;
}