#include "regex_parser.h"

// 在需要的位置插入显式连接符号
void RegexParser::insertExplicitConcat()
{
    string result;
    for (size_t i = 0; i < regex.length(); i++)
    {
        result += regex[i];
        if (i + 1 < regex.length())
        {
            char curr = regex[i];
            char next = regex[i + 1];
            // 在以下情况需要插入连接符
            if ((curr == '0' || curr == '1' || curr == ')' || curr == '*') &&
                (next == '0' || next == '1' || next == '('))
            {
                result += '.'; // 使用'.'表示显式连接
            }
        }
    }
    regex = result;
}

// 解析表达式（处理并运算+）
Node *RegexParser::parseExpr()
{
    Node *left = parseTerm();

    while (pos < regex.length() && regex[pos] == '+')
    {
        pos++;
        Node *right = parseTerm();
        Node *unionNode = new Node(NodeType::UNION);
        unionNode->left = left;
        unionNode->right = right;
        left = unionNode;
    }

    return left;
}

// 解析项（处理连接）
Node *RegexParser::parseTerm()
{
    Node *left = parseFactor();

    while (pos < regex.length() &&
           regex[pos] != '+' &&
           regex[pos] != ')')
    {
        Node *right = parseFactor();
        Node *concatNode = new Node(NodeType::CONCAT);
        concatNode->left = left;
        concatNode->right = right;
        left = concatNode;
    }

    return left;
}

// 解析因子（处理基本符号、括号和星号）
Node *RegexParser::parseFactor()
{
    Node *node = nullptr;

    if (pos >= regex.length())
    {
        return nullptr;
    }

    char c = regex[pos];
    pos++;

    if (c == '(')
    {
        node = parseExpr();
        pos++; // 跳过')'
    }
    else if (c == '0' || c == '1')
    {
        node = new Node(c);
    }

    // 处理星号
    while (pos < regex.length() && regex[pos] == '*')
    {
        Node *starNode = new Node(NodeType::STAR);
        starNode->left = node;
        node = starNode;
        pos++;
    }

    return node;
}

// 主解析函数
Node *RegexParser::parse(string re)
{
    regex = re;
    pos = 0;
    insertExplicitConcat();
    return parseExpr();
}