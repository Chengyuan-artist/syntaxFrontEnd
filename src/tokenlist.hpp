//
// Created by 张承元 on 2021/3/17.
//
#include "token.hpp"


#ifndef SYNTAXFRONTEND_TOKENLIST_HPP
#define SYNTAXFRONTEND_TOKENLIST_HPP

#define Init_TokenList_Length 50
#define Add_TokenList_Length 20


typedef struct TokenList {
    Token * *val;
    int len;
    int store_size;
    int then_p;
} TokenList;


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


// 在token_list后加入新Token指针
void AddToken(TokenList *token_list, Token *token);


// Public
// TokenList的成员函数，返回下一个Token*，并指针后移
Token *NextToken(TokenList *list);

// TokenList的成员函数，返回当前Token*
Token *CurrentToken(TokenList *list);

// 得到与then_p相对位置为index的token指针
Token *TokenAt(TokenList *list, int index);


#endif //SYNTAXFRONTEND_TOKENLIST_HPP
