//
// Created by 张承元 on 2021/3/8.
//

#include "node_re.hpp"
#include "lexical_re.hpp"
#include "parser_re.hpp"

using namespace std;
int main(){
    Parser *parser = GetParser("test.c");

    LexicalAnalyse(parser);
//    PreProcess(parser);

//    DisplayAllToken(parser);

    Parsing(parser);

    Format(parser, "out.c");

    ReleaseParser(parser);

    return 0;
}