//
// Created by 张承元 on 2021/3/17.
//

#include "token.hpp"
#include "tokenlist.hpp"
#include <cstdio>
#include <cctype>
#include <cstring>
#include <cstdlib>

#ifndef SYNTAXFRONTEND_LEXICAL_HPP
#define SYNTAXFRONTEND_LEXICAL_HPP

// 判断ch是否为16进制合法数
int is_Ox(char ch);

// 检查token_text是否为关键字
TokenType check_keyword();

// 检查常量后缀
TokenType const_suffix(char ch);

TokenType gettoken(FILE *fp);

// Public
Token* GetToken(FILE *in);

// Public
TokenList* GetTokenList(FILE *in);


#endif //SYNTAXFRONTEND_LEXICAL_HPP
