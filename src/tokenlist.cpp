//
// Created by 张承元 on 2021/3/17.
//

#include "tokenlist.hpp"

Token *NextToken(TokenList *list) {
    (list->then_p)++;
    return CurrentToken(list);
}

Token *CurrentToken(TokenList *list) {
    return TokenAt(list, 0);
}

void AddToken(TokenList *token_list, Token *token) {
    if (token_list->len >= token_list->store_size) {
        token_list->store_size += Add_TokenList_Length;

        token_list->val = (Token **) realloc(token_list->val,
                                             sizeof(Token *) * (token_list->store_size));
    }
    token_list->val[token_list->len++] = token;
}

TokenList *getTokenList() {
    TokenList *token_list = (TokenList *) malloc(sizeof(TokenList));
    token_list->val = (Token **) malloc(sizeof(Token *) * Init_TokenList_Length);
    token_list->len = 0;
    token_list->store_size = Init_TokenList_Length;
    token_list->then_p = 0;
    return token_list;
}

Token *TokenAt(TokenList *list, int index){
    int now_p = list->then_p + index;
    if (now_p < 0 || now_p >= list->len){
        return nullptr;
    }
    return (list->val)[now_p];
}


TokenList *InsertList(TokenList *target_list, TokenList *insert_list, int insert_pos) {

    if (insert_pos >= target_list->len) insert_pos = target_list->len;

    TokenList *new_list = getTokenList();

    for (int i = 0; i < insert_pos; ++i) {
        AddToken(new_list, TokenAt(target_list, i));
    }
    for (int i = 0; i < insert_list->len; ++i) {
        AddToken(new_list, TokenAt(insert_list, i));
    }
    for (int i = insert_pos; i < target_list->len; ++i) {
        AddToken(new_list, TokenAt(target_list, i));
    }

    // 假删除
    DeleteTokenList(target_list);
    DeleteTokenList(insert_list);

    return new_list;
}

int DeleteToken(TokenList *list, int delete_pos) {
    if (delete_pos< 0 || delete_pos >= list->len) {
        return 0;
    }
    list->len--;
    // really release the token
    // 防止内存泄漏
    Token *token = TokenAt(list, delete_pos);
    free(token);
    for (int i = delete_pos; i < list->len; ++i) {
        list->val[i] = list->val[i+1];
    }
    return 1;
}

int DeleteTokenList(TokenList *list) {
    if (list == nullptr) return 0;
    free(list->val);
    free(list);
    return 1;
}

int ReleaseTokenList(TokenList *list) {
    if (list == nullptr) return 0;
    Token *token;
    for (int i = 0; i < list->len; ++i) {
        token = TokenAt(list, i);
        free(token);
    }
    DeleteTokenList(list);
    return 1;
}
