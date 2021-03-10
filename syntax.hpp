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
    Eof,
    ERROR_TOKEN, Identifier, INT_CONST, LONG_CONST, FLOAT_CONST, CHAR_CONST,
    INT, LONG, FLOAT, DOUBLE, CHAR, IF, ELSE, WHILE, FOR, RETURN, BREAK, CONTINUE,
    LSP, RSP, LCP, RCP, SEMI, COMMA,
    ASSIGN, LP, RP, AndAnd, OrOr, PLUS, MINUS, MULTIPLY, DIVIDE, MOD, EQ, UEQ, Clt, Cle, Igt, Ige,
    NoType,
    Program, ExtDefList, ExtVarDef, FuncDef, DeclaratorList, FormalParaList, FormalPara,
    CompoundSentenceList, LocalVarDef,
    SingleExpStatement, ReturnStatement, IfStatement, IfElseStatement,
    Begin_End, Brackets
};

typedef struct node{
    token_kind type;
    union {
        struct node* children[4];
        char text[20];  // IDENT
    }val;
}Node;

Node *getNode();

token_kind gettoken(FILE *fp);

Node *program(FILE *in);

int precede(int op, int last_op);