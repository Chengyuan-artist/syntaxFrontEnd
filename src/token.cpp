//
// Created by 张承元 on 2021/3/17.
//
#include "token.hpp"

Token *GetToken() {
    Token *token = (Token *) malloc(sizeof(Token));
    memset(token, 0, sizeof(Token));
    return token;
}


char token_type_string[50][30] = {
        "ERROR_TOKEN", "Identifier", "INT_CONST", "LONG_CONST", "FLOAT_CONST", "CHAR_CONST",
        "int ", "long ", "float ", "double ", "char ", "if", "else", "while", "for", "return", "break",
        "continue",
        "[", "]", "{", "}", ";", ",",
        " = ", "(", ")", " && ", " || ", " + ", " - ", " * ", " / ", " % ", " == ", " != ", " < ", " <= ", " > ", " >= ",
        "Begin_Op",
        "Eof", "#include", "#define","Annotation"
};


char *ToString(enum TokenType type){
    return token_type_string[type];
};


int isConstant(enum TokenType type) {
    if (type == INT_CONST ||
        type == FLOAT_CONST ||
        type == CHAR_CONST ||
        type == LONG_CONST) {
        return 1;
    }
    return 0;
}