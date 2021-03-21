//
// Created by 张承元 on 2021/3/17.
//
#include "lexical.hpp"


#define keyword_num 12
char keyword[20][10] = {"int", "long", "float", "double", "char", "if", "else", "while", "for", "return", "break",
                        "continue",};
char token_text[500];
int token_text_len = 0;
int then_row = 1; // 当前行数


int is_Ox(char ch) {
    if (isdigit(ch) || (ch >= 'a' && ch <= 'f') || ((ch >= 'A' && ch <= 'F'))) return 1;
    return 0;
}


TokenType check_keyword() {
    // 查找表查找关键字 否则返回 IDENT
    for (int i = 0; i < keyword_num; ++i) {
        if (strcmp(token_text, keyword[i]) == 0)
            return TokenType(INT + i);
    }
    return Identifier;
}


TokenType const_suffix(char ch) {
    if (ch == 'l' || ch == 'L') {
        token_text[token_text_len++] = ch;
        token_text[token_text_len] = '\0';
        return LONG_CONST;
    }
    if (ch == 'f' || ch == 'F') {
        token_text[token_text_len++] = ch;
        token_text[token_text_len] = '\0';
        return FLOAT_CONST;
    }
    return ERROR_TOKEN;
}

Token *GetToken(FILE *fp) {
    Token *token = (Token *) malloc(sizeof(Token));
    token->type = gettoken(fp);
    token->then_row = then_row;
    memset(token->text, 0, sizeof(token->text));

    if (token->type == Identifier
        || token->type == INT_CONST
        || token->type == FLOAT_CONST
        || token->type == LONG_CONST
        || token->type == CHAR_CONST
        || token->type == INCLUDE
        || token->type == DEFINE
        || token->type == Annotation) {
        strcpy(token->text, token_text);
    }

    //ERROR_TOKEN catch
    if (token->type == ERROR_TOKEN) {
        char tmp[50];
        fgets(tmp, sizeof(tmp), fp);  //读取一行
        then_row++;
    }

    return token;
}

TokenList *GetTokenList(FILE *in) {
    then_row = 1;
    TokenList *tokenList = getTokenList();

    Token *token = GetToken(in);
    while (token->type != Eof) {
        AddToken(tokenList, token);
        token = GetToken(in);
    }
    AddToken(tokenList, token); // 添加结尾结点

    tokenList->then_p = 0;
    return tokenList;
}

//token_text
//then_row
TokenType gettoken(FILE *fp) {

    token_text_len = 0;
    char ch;

    do {
        ch = getc(fp);
        if (ch == '\n') ++then_row;
    } while (ch == ' ' || ch == '\n');

    if (ch == EOF) return Eof;

    if (isalpha(ch)) { //如果ch是字母
        do {
            token_text[token_text_len++] = ch;
            ch = fgetc(fp);
        } while (isalpha(ch) || isdigit(ch));

        ungetc(ch, fp);

        token_text[token_text_len] = '\0';
        return check_keyword();
    }
    if (isdigit(ch)) { //如果ch是数字
        if (ch == '0') {
            token_text[token_text_len++] = ch;
            ch = fgetc(fp);
            if (ch == 'x') {
                token_text[token_text_len++] = ch;
                ch = fgetc(fp);
                if (is_Ox(ch)) {//属于16进制数
                    do {
                        token_text[token_text_len++] = ch;
                        ch = fgetc(fp);
                    } while (is_Ox(ch));
                    if (isalpha(ch)) {//ch是其他字母
                        // 常量后缀
                        return const_suffix(ch);
                    }
                    //ch是其他符号
                    ungetc(ch, fp);
                    token_text[token_text_len] = '\0';
                    return INT_CONST;
                }
                //token_text: 0x
                //ch不属于16进制数
                return ERROR_TOKEN;
            }
            //token_text: 0
            //ch != 'x'
            if (isalpha(ch)) {
                // 常量后缀
                return const_suffix(ch);
            }
            //ch is not alpha
            if (ch >= '0' && ch <= '7') {
                do {
                    token_text[token_text_len++] = ch;
                    ch = fgetc(fp);
                } while (ch >= '0' && ch <= '7');
                if (ch == '8' || ch == '9') {
                    return ERROR_TOKEN;
                }
                ungetc(ch, fp);
                token_text[token_text_len] = '\0';
                return INT_CONST;
            }
            // ch is not alpha and also not '8' or '9'
            if (ch == '.') { // 处理小数点
                do {
                    token_text[token_text_len++] = ch;
                    ch = fgetc(fp);
                } while (isdigit(ch));
                if (isalpha(ch)) {
                    // 后缀处理
                    return const_suffix(ch);
                }
                // 其他字符
                ungetc(ch, fp);
                token_text[token_text_len] = '\0';
                return FLOAT_CONST;
            }
            // 其他字符
            ungetc(ch, fp);
            token_text[token_text_len] = '\0';
            return INT_CONST;

        } else {//ch != '0' 非0数字
            do {
                token_text[token_text_len++] = ch;
                ch = fgetc(fp);
            } while (isdigit(ch));
            if (isalpha(ch)) {
                // 后缀处理
                return const_suffix(ch);
            }
            if (ch == '.') { // 处理小数点
                do {
                    token_text[token_text_len++] = ch;
                    ch = fgetc(fp);
                } while (isdigit(ch));
                if (isalpha(ch)) {
                    // 后缀处理
                    return const_suffix(ch);
                }
                // 其他字符
                ungetc(ch, fp);
                token_text[token_text_len] = '\0';
                return FLOAT_CONST;
            }
            // 其他字符
            ungetc(ch, fp);
            token_text[token_text_len] = '\0';
            return INT_CONST;
        }
    }

    switch (ch) {
        case '#':
            ch = getc(fp);
            while (isalpha(ch)) {
                token_text[token_text_len++] = ch;
                ch = fgetc(fp);
            }
            token_text[token_text_len] = '\0';
            while (ch == ' ') { // 去除空格
                ch = getc(fp);
            }

            if (strcmp(token_text, "include") == 0) {
                if (ch != '\"') {
                    return ERROR_TOKEN;
                }
                token_text_len = 0;

                // puts in a word
                ch = getc(fp);
                while (isalpha(ch) || ch == '.' || isdigit(ch)) {
                    token_text[token_text_len++] = ch;
                    ch = fgetc(fp);
                }
                if (ch != '\"') {
                    return ERROR_TOKEN;
                }
                token_text[token_text_len] = '\0';
                return INCLUDE;
            }
            if (strcmp(token_text, "define") == 0) {
                fgets(token_text, 50, fp);
                // 有无输出错误？
                // 已检查
                then_row++;
                return DEFINE;
            }
            return ERROR_TOKEN;
        case '(':
            return LP;
        case ')':
            return RP;
        case '[':
            return LSP;
        case ']':
            return RSP;
        case '{':
            return LCP;
        case '}':
            return RCP;
        case ';':
            return SEMI;
        case ',':
            return COMMA;

        case '&':
            ch = getc(fp);
            if (ch == '&') return AndAnd;
            else {
                ungetc(ch, fp);
                return ERROR_TOKEN;
            }
        case '|':
            ch = getc(fp);
            if (ch == '|') return OrOr;
            else {
                ungetc(ch, fp);
                return ERROR_TOKEN;
            }

        case '+':
            return PLUS;
        case '-':
            return MINUS;
        case '*':
            return MULTIPLY;
        case '/':
            ch = fgetc(fp);
            if (ch == '/') {
                ungetc(ch, fp);
                ungetc(ch ,fp);
                fgets(token_text, 50, fp);
                then_row ++;
                return Annotation;
            }
            if (ch == '*') {
                token_text[token_text_len++] = '/';
                token_text[token_text_len++] = '*';
                ch = getc(fp);
                char ch_next = getc(fp);
                while (ch != '*' || ch_next != '/'){

                    token_text[token_text_len++] = ch;

                    if (ch == '\n') then_row++;
                    if (ch_next == EOF) return ERROR_TOKEN;
                    ch = ch_next;
                    ch_next = getc(fp);
                }
                token_text[token_text_len++] = ch;
                token_text[token_text_len++] = ch_next;
                token_text[token_text_len++] = '\n';
                token_text[token_text_len] = '\0';

                return Annotation;
            }
            ungetc(ch,fp);
            return DIVIDE;
        case '%':
            return MOD;

        case '>':
            ch = getc(fp);
            if (ch == '=')return Ige;
            else {
                ungetc(ch, fp);
                return Igt;
            }
        case '<':
            ch = getc(fp);
            if (ch == '=')return Cle;
            else {
                ungetc(ch, fp);
                return Clt;
            }
        case '=':
            ch = getc(fp);
            if (ch == '=') return EQ;
            else {
                ungetc(ch, fp);
                return ASSIGN;
            }
        case '!':
            ch = getc(fp);
            if (ch == '=') return UEQ;
            else {
                ungetc(ch, fp);
                return ERROR_TOKEN;
            }
        case '\'':
            //  字符常量识别
            ch = getc(fp);
            if (ch == '\\') {
                token_text[token_text_len++] = ch;
                ch = getc(fp);
                if (ch == 'a' || ch == 'b' || ch == 'f' || ch == 'n' || ch == 'r' || ch == 't' || ch == 'v' ||
                    ch == '\\' || ch == '\'' || ch == '\"' || ch == '?') {
                    token_text[token_text_len++] = ch;
                    ch = getc(fp);
                    if (ch == '\'') {
                        token_text[token_text_len] = '\0';
                        return CHAR_CONST;
                    }
                    return ERROR_TOKEN;
                }
                if (ch == '0') {
                    token_text[token_text_len++] = ch;
                    ch = getc(fp);
                    if (ch == '\'') {
                        token_text[token_text_len] = '\0';
                        return CHAR_CONST;
                    }
                }
                if (ch >= '0' && ch <= '7') {
                    do {
                        token_text[token_text_len++] = ch;
                        ch = getc(fp);
                    } while (ch >= '0' && ch <= '7');
                    if (token_text_len == 4 && ch == '\'') {
                        token_text[token_text_len] = '\0';
                        return CHAR_CONST;
                    }
                    return ERROR_TOKEN;
                }
                if (ch == 'x') {
                    do {
                        token_text[token_text_len++] = ch;
                        ch = getc(fp);
                    } while (is_Ox(ch));
                    if (token_text_len == 4 && ch == '\'') {
                        token_text[token_text_len] = '\0';
                        return CHAR_CONST;
                    }
                    return ERROR_TOKEN;
                }
                return ERROR_TOKEN;
            }
            if (ch == '\'' || ch == '\"') return ERROR_TOKEN;
            else {
                token_text[token_text_len++] = ch;
                ch = getc(fp);
                if (ch == '\'') {
                    token_text[token_text_len] = '\0';
                    return CHAR_CONST;
                }
                return ERROR_TOKEN;
            }

        default:
            return ERROR_TOKEN;
    }
    return ERROR_TOKEN;
}
