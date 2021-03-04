//
// Created by 张承元 on 2021/3/2.
//

#ifndef SYNTAXFRONTEND_SYNTAX_H
#define SYNTAXFRONTEND_SYNTAX_H

#endif //SYNTAXFRONTEND_SYNTAX_H

#include <iostream>
#include <cstdio>
#include <cstring>

enum token_kind {
    ERROR_TOKEN, IDENT, INT_CONST, LONG_CONST, FLOAT_CONST, CHAR_CONST,
    INT, LONG, FLOAT, DOUBLE, CHAR, IF, ELSE, WHILE, FOR, RETURN, BREAK, CONTINUE,
    ASSIGN, LP, RP, LSP, RSP, LCP, RCP, SEMI, COMMA, AndAnd, OrOr, PLUS, MINUS, MUTIPLY, DIVIDE, MOD,
    EQ, UEQ, Clt, Cle, Igt, Ige, Program, ExtDefList, ExtVarDef, FuncDef, VarList, FormalParaList, FormalPara
};

typedef struct node{
    int type;
    union {
        struct node* children[3];
        char text[20];  // IDENT
    }val;
}Node;

int gettoken(FILE *fp);