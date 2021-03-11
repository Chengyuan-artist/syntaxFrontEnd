//
// Created by 张承元 on 2021/3/8.
//

#include "node_re.hpp"
#include "lexical_re.hpp"
#include "parser_re.hpp"

#include <iostream>
using namespace std;
int main(){
    Parser *parser = GetParser("test.c");
    LexicalAnalyse(parser);
    Parsing(parser);

    Format(parser, "out.c");
    visit(parser->root, 0);
    return 0;
}