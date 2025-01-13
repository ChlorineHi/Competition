#include "regex_parser.h"

int main()
{
    RegexParser parser;
    string regex = "10*(0+1)1*";
    Node *root = parser.parse(regex);

    // 后续处理...
    return 0;
}