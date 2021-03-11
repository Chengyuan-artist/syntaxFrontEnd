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


#define Init_TokenList_Length 50
#define Add_TokenList_Length 20
typedef struct TokenList {
   Token * *val;
   int len;
   int store_size;
   int then_p;
} TokenList;


char *ToString(enum TokenType type);

int isConstant(enum TokenType type);

TokenList* getTokenList();

TokenList* InsertList(TokenList *target_list, TokenList *insert_list, int insert_pos);

int DeleteToken(TokenList *list, int delete_pos);

// 仅仅删除TokenList,不释放所指的token
int DeleteTokenList(TokenList *list);

// 真删除
int ReleaseTokenList(TokenList *list);

void AddToken(TokenList *token_list, Token *token);

// new
Token* GetToken();

// Public
Token* GetToken(FILE *in);

// Public
TokenList* GetTokenList(FILE *in);

// Public
Token *NextToken(TokenList *list);

Token *CurrentToken(TokenList *list);

// 得到与then_p相对位置为index的token指针
Token *TokenAt(TokenList *list, int index);


int is_Ox(char ch);

TokenType check_keyword();

TokenType const_suffix(char ch);

TokenType gettoken(FILE *fp);


#endif //SYNTAXFRONTEND_LEXICAL_HPP



