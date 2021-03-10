//
// Created by 张承元 on 2021/3/2.
//

#include "syntax.hpp"
#include "refactor/lexical_re.hpp"


#define keyword_num 12

char keyword[20][10] = {"int", "long", "float", "double", "char", "if", "else", "while", "for", "return", "break",
                        "continue",};
char token_text[20];
int then_row = 0; // 当前行数

int is_Ox(char ch) {
    if (isdigit(ch) || (ch >= 'a' && ch <= 'f') || ((ch >= 'A' && ch <= 'F'))) return 1;
    return 0;
}

token_kind check_keyword() {
    // 查找表查找关键字 否则返回 IDENT
    for (int i = 0; i < keyword_num; ++i) {
        if (strcmp(token_text, keyword[i]) == 0)
            return token_kind(INT + i);
    }
    return Identifier;
}

token_kind const_suffix(char ch) {
    if (ch == 'l' || ch == 'L') return LONG_CONST;
    if (ch == 'f' || ch == 'F') return FLOAT_CONST;
    return ERROR_TOKEN;
}

token_kind gettoken(FILE *fp) {

    int len = 0;
    char ch;

    do {
        ch = getc(fp);
        if (ch == '\n') ++then_row;
    } while (ch == ' ' || ch == '\n');

    if (ch == EOF) return Eof;

    if (isalpha(ch)) { //如果ch是字母
        do {
            token_text[len++] = ch;
            ch = fgetc(fp);
        } while (isalpha(ch) || isdigit(ch));

        ungetc(ch, fp);

        token_text[len] = '\0';
        return check_keyword();
    }
    if (isdigit(ch)) { //如果ch是数字
        if (ch == '0') {
            token_text[len++] = ch;
            ch = fgetc(fp);
            if (ch == 'x') {
                token_text[len++] = ch;
                ch = fgetc(fp);
                if (is_Ox(ch)) {//属于16进制数
                    do {
                        token_text[len++] = ch;
                        ch = fgetc(fp);
                    } while (is_Ox(ch));
                    if (isalpha(ch)) {//ch是其他字母
                        // 常量后缀
                        return const_suffix(ch);
                    }
                    //ch是其他符号
                    ungetc(ch, fp);
                    token_text[len] = '\0';
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
                    token_text[len++] = ch;
                    ch = fgetc(fp);
                } while (ch >= '0' && ch <= '7');
                if (ch == '8' || ch == '9') {
                    return ERROR_TOKEN;
                }
                ungetc(ch, fp);
                token_text[len] = '\0';
                return INT_CONST;
            }
            // ch is not alpha and also not '8' or '9'
            if (ch == '.') { // 处理小数点
                do {
                    token_text[len++] = ch;
                    ch = fgetc(fp);
                } while (isdigit(ch));
                if (isalpha(ch)) {
                    // 后缀处理
                    return const_suffix(ch);
                }
                // 其他字符
                ungetc(ch, fp);
                token_text[len] = '\0';
                return FLOAT_CONST;
            }
            // 其他字符
            ungetc(ch, fp);
            token_text[len] = '\0';
            return INT_CONST;

        } else {//ch != '0' 非0数字
            do {
                token_text[len++] = ch;
                ch = fgetc(fp);
            } while (isdigit(ch));
            if (isalpha(ch)) {
                // 后缀处理
                return const_suffix(ch);
            }
            if (ch == '.') { // 处理小数点
                do {
                    token_text[len++] = ch;
                    ch = fgetc(fp);
                } while (isdigit(ch));
                if (isalpha(ch)) {
                    // 后缀处理
                    return const_suffix(ch);
                }
                // 其他字符
                ungetc(ch, fp);
                token_text[len] = '\0';
                return FLOAT_CONST;
            }
            // 其他字符
            ungetc(ch, fp);
            token_text[len] = '\0';
            return INT_CONST;
        }
    }

    switch (ch) {
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
            // TODO 负数处理
        case '*':
            return MULTIPLY;
        case '/':
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
                token_text[len++] = ch;
                ch = getc(fp);
                if (ch == 'a' || ch == 'b' || ch == 'f' || ch == 'n' || ch == 'r' || ch == 't' || ch == 'v' ||
                    ch == '\\' || ch == '\'' || ch == '\"' || ch == '?') {
                    token_text[len++] = ch;
                    ch = getc(fp);
                    if (ch == '\'') {
                        token_text[len] = '\0';
                        return CHAR_CONST;
                    }
                    return ERROR_TOKEN;
                }
                if (ch == '0') {
                    token_text[len++] = ch;
                    ch = getc(fp);
                    if (ch == '\'') {
                        token_text[len] = '\0';
                        return CHAR_CONST;
                    }
                }
                if (ch >= '0' && ch <= '7') {
                    do {
                        token_text[len++] = ch;
                        ch = getc(fp);
                    } while (ch >= '0' && ch <= '7');
                    if (len == 4 && ch == '\'') {
                        token_text[len] = '\0';
                        return CHAR_CONST;
                    }
                    return ERROR_TOKEN;
                }
                if (ch == 'x') {
                    do {
                        token_text[len++] = ch;
                        ch = getc(fp);
                    } while (is_Ox(ch));
                    if (len == 4 && ch == '\'') {
                        token_text[len] = '\0';
                        return CHAR_CONST;
                    }
                    return ERROR_TOKEN;
                }
                return ERROR_TOKEN;
            }
            if (ch == '\'' || ch == '\"') return ERROR_TOKEN;
            else {
                token_text[len++] = ch;
                ch = getc(fp);
                if (ch == '\'') {
                    token_text[len] = '\0';
                    return CHAR_CONST;
                }
                return ERROR_TOKEN;
            }

        default:
            return ERROR_TOKEN;
    }
    return ERROR_TOKEN;
}


