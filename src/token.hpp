//
// Created by 张承元 on 2021/3/17.
//
#include <cstdlib>
#include <cstring>


#ifndef SYNTAXFRONTEND_TOKEN_HPP
#define SYNTAXFRONTEND_TOKEN_HPP

enum TokenType {
    ERROR_TOKEN, Identifier, INT_CONST, LONG_CONST, FLOAT_CONST, CHAR_CONST,
    INT, LONG, FLOAT, DOUBLE, CHAR, IF, ELSE, WHILE, FOR, RETURN, BREAK, CONTINUE,
    LSP, RSP, LCP, RCP, SEMI, COMMA,
    ASSIGN, LP, RP, AndAnd, OrOr, PLUS, MINUS, MULTIPLY, DIVIDE, MOD, EQ, UEQ, Clt, Cle, Igt, Ige,
    Begin_Op,
    Eof, INCLUDE, DEFINE, Annotation
};


#define Token_Text_Max_Len 500

typedef struct Token {
    enum TokenType type;
    int then_row;
    char text[Token_Text_Max_Len];
} Token;


// new Token 得到一个初始化的Token指针
Token* GetToken();

char *ToString(enum TokenType type);

int isConstant(enum TokenType type);





#endif //SYNTAXFRONTEND_TOKEN_HPP
