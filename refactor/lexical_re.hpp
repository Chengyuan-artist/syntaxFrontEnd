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

// 将 TokenType 转化为相应的字符指针
char *ToString(enum TokenType type);

// 判断 TokenType 是否为常量类型
int isConstant(enum TokenType type);

// TokenList的成员函数，初始化TokenList
TokenList* getTokenList();

// TokenList的成员函数，将insert_list插入至target_list的insert_pos位置处，
// 返回值为插入后的TokenList*
TokenList* InsertList(TokenList *target_list, TokenList *insert_list, int insert_pos);

//  TokenList的成员函数，删除delete_pos位置处的Token*，并释放所指向的空间
int DeleteToken(TokenList *list, int delete_pos);

// 仅仅删除TokenList,不释放所指的token
int DeleteTokenList(TokenList *list);

// 真删除,释放TokenList中全部Token指针所指向的内存
int ReleaseTokenList(TokenList *list);

void AddToken(TokenList *token_list, Token *token);

// new Token 得到一个初始化的Token指针
Token* GetToken();

// Public
Token* GetToken(FILE *in);

// Public
TokenList* GetTokenList(FILE *in);

// Public
// TokenList的成员函数，返回下一个Token*，并指针后移
Token *NextToken(TokenList *list);

// TokenList的成员函数，返回当前Token*
Token *CurrentToken(TokenList *list);

// 得到与then_p相对位置为index的token指针
Token *TokenAt(TokenList *list, int index);

// 判断ch是否为16进制合法数
int is_Ox(char ch);

// 检查token_text是否为关键字
TokenType check_keyword();

// 检查常量后缀
TokenType const_suffix(char ch);

TokenType gettoken(FILE *fp);


#endif //SYNTAXFRONTEND_LEXICAL_HPP



