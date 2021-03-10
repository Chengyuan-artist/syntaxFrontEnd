//
// Created by 张承元 on 2021/3/8.
//

#include "node_re.hpp"
#include "lexical_re.hpp"
#include "parser_re.hpp"

#include <iostream>

int main(){
    FILE *in = fopen("test.in", "r");
    Parser *parser = GetParser(in);

    LexicalAnalyse(parser);
    Parsing(parser);

    return 0;
}