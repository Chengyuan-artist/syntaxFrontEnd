//
// Created by 张承元 on 2021/3/8.
//

#include "parser.hpp"

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