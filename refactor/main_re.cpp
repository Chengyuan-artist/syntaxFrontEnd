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

    PreProcess(parser);

    Parsing(parser);
    visit(parser->root, 0);

//    Parsing(parser);
//
//    visit(parser->root, 0);
//    char a[100];
//    fgets(a, 100, in);
//    printf("%s", a);
    return 0;
}