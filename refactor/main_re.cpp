//
// Created by 张承元 on 2021/3/8.
//

#include "node_re.hpp"
#include "lexical_re.hpp"
#include "parser_re.hpp"

#include <iostream>
using namespace std;
int main(){
    FILE *in = fopen("test.c", "r");
    Parser *parser = GetParser(in);

    LexicalAnalyse(parser);
    Parsing(parser);


    visit(parser->root, 0);
//    char *a = ToString(LCP), *b = ToString(PLUS);
//    printf("%s %s", ToString(LCP), b);
    return 0;
}