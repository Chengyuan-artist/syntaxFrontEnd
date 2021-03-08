//
// Created by 张承元 on 2021/3/7.
//


#include <cctype>
#include <cstring>
#include <cstdlib>
#include <cstdio>


#ifndef SYNTAXFRONTEND_LEXICAL_HPP
#define SYNTAXFRONTEND_LEXICAL_HPP

enum TokenType {
    ERROR_TOKEN, IDENT, INT_CONST, LONG_CONST, FLOAT_CONST, CHAR_CONST,
    INT, LONG, FLOAT, DOUBLE, CHAR, IF, ELSE, WHILE, FOR, RETURN, BREAK, CONTINUE,
    LSP, RSP, LCP, RCP, SEMI, COMMA,
    ASSIGN, LP, RP, AndAnd, OrOr, PLUS, MINUS, MULTIPLY, DIVIDE, MOD, EQ, UEQ, Clt, Cle, Igt, Ige,
    Eof
};


#define Token_Text_Max_Len 20

typedef struct Token {
    enum TokenType type;
    int then_row;
    char text[Token_Text_Max_Len];
} Token;

typedef struct TokenListNode {
    Token *token;
    struct TokenListNode *next;
} TokenListNode;

TokenListNode *getTokenListNode();


typedef struct TokenList {
    TokenListNode *head;
    TokenListNode *tail;
    TokenListNode *then_p;
} TokenList;

TokenList* getTokenList(
        TokenListNode *head,
        TokenListNode *tail);

// Public
Token *GetToken(FILE *in);

// Public
TokenList *GetTokenList(FILE *in);

// Public
Token *NextToken(TokenList *list);

Token *CurrentToken(TokenList *list);


int is_Ox(char ch);

TokenType check_keyword();

TokenType const_suffix(char ch);

TokenType gettoken(FILE *fp);


#endif //SYNTAXFRONTEND_LEXICAL_HPP



