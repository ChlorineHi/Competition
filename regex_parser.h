#include <string>
#include <vector>
#include <map>
#include <set>
using namespace std;

// 节点类型枚举
enum class NodeType
{
    CHAR,   // 字符节点 (0 或 1)
    CONCAT, // 连接
    UNION,  // 并运算 (+)
    STAR    // 闭包 (*)
};

// 语法树节点
class Node
{
public:
    NodeType type;
    char value; // 当type为CHAR时使用
    Node *left;
    Node *right;

    Node(NodeType t) : type(t), value('\0'), left(nullptr), right(nullptr) {}
    Node(char v) : type(NodeType::CHAR), value(v), left(nullptr), right(nullptr) {}
};

// 正则表达式解析器
class RegexParser
{
private:
    string regex;
    int pos;

    // 辅助函数
    Node *parseExpr();
    Node *parseTerm();
    Node *parseFactor();
    void insertExplicitConcat();

public:
    RegexParser() : pos(0) {}
    Node *parse(string re);
};