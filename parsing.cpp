//
// Created by 张承元 on 2021/3/2.
//

#include "syntax.h"

#define CHECK_ERROR if(error)return nullptr;
#define REPORT_ERROR_AND_RETURN error = 1;return nullptr;

int error = 0;
int error_type;

extern char token_text[20];
extern int then_row; // 当前行数

FILE *fp;// init in main
int token_type; //value from gettoken

// tmp var to save the last res form gettoken
int tmp_type;
char tmp_token[20];

Node *extDefList();

Node *extDef();

Node *extVarDef();

Node *funcDef();

Node *varList();

Node *formalParaList();

Node *formalPara();

Node *compoundSentenceList();

Node *compoundSentence();

Node *localVarDef();

Node *statement();

Node *varList_up();

Node *expression();

Node *program() {
    Node *root = getNode();
    root->type = Program;

    token_type = gettoken(fp);
    root->val.children[0] = extDefList();

    // TODO 语法错误传递

    return root;
}

Node *extDefList() {
    if (token_type == EOF) return nullptr;

    Node *root = getNode();
    root->type = ExtDefList;
    root->val.children[0] = extDef();
    root->val.children[1] = extDefList();
    return root;
}

Node *extDef() {
    if (token_type < INT || token_type > CHAR) {
        REPORT_ERROR_AND_RETURN
    }
    tmp_type = token_type;

    token_type = gettoken(fp);
    if (token_type != IDENT) {
        REPORT_ERROR_AND_RETURN
    }

    strcpy(tmp_token, token_text);

    token_type = gettoken(fp);
    if (token_type != LP) {
        return extVarDef();
    } else return funcDef();

}


Node *extVarDef() {
    CHECK_ERROR
    Node *root = getNode();
    root->type = ExtVarDef;

    root->val.children[0] = getNode();
    root->val.children[0]->type = tmp_type;

    root->val.children[1] = varList();

    return root;
}

Node *varList() {
    CHECK_ERROR
    // 调用前以确保 tmp_token 存有var的变量名
    if (token_type != COMMA && token_type != SEMI) {
        REPORT_ERROR_AND_RETURN
    }

    Node *root = getNode();
    root->type = VarList;

    // 左孩子
    Node *lc = getNode();
    lc->type = IDENT;
    strcpy(lc->val.text, tmp_token);
    root->val.children[0] = lc;


    if (token_type == COMMA) {
        token_type = gettoken(fp);
        if (token_type != IDENT) {
            free(lc);
            free(root);
            REPORT_ERROR_AND_RETURN
        }
        // 保存变量名
        strcpy(tmp_token, token_text);
        token_type = gettoken(fp);
        root->val.children[1] = varList(); //递归调用
        return root;
    }
    if (token_type == SEMI) {
        token_type = gettoken(fp);
        root->val.children[1] = nullptr;
        return root;
    }
    return nullptr;
}

// <函数定义>：：=<类型说明符> <函数名>（<形式参数序列>）{<复合语句序列>}  (可无)
Node *funcDef() {
    CHECK_ERROR
    Node *root = getNode();
    root->type = FuncDef;

    Node *type = getNode();
    type->type = tmp_type;
    root->val.children[0] = type;

    token_type = gettoken(fp);
    root->val.children[1] = formalParaList();

    token_type = gettoken(fp);
    if (token_type == SEMI) {
        root->val.children[2] = nullptr;
        return root;
    }
    if (token_type != LCP) {
        REPORT_ERROR_AND_RETURN
    }
    // token_type == {
    token_type = gettoken(fp);
    root->val.children[2] = compoundSentenceList();

    return root;
}

// <复合语句序列> ::= <复合语句>  <复合语句序列>  |  <复合语句>
Node *compoundSentenceList() {
    CHECK_ERROR
    if (token_type == RCP) return nullptr;

    Node *root = getNode();
    root->type = CompoundSentenceList;

    root->val.children[0] = compoundSentence();
    root->val.children[1] = compoundSentenceList();

    return root;
}

// <复合语句> ::= <局部变量定义>  |  <语句>
Node *compoundSentence() { // 需判断
    CHECK_ERROR
    if (token_type >= INT && token_type <= CHAR) { // 是 <类型说明符>
        return localVarDef();
    } else {
        // 做一次 token 缓存 ---不需要?
        tmp_type = token_type;
        strcpy(tmp_token, token_text);
        return statement();
    }
}

/* <语句> ::= <表达式>; | return  <表达式>;
 *         | {<复合语句序列>}
           | if (<表达式>) <语句>
		   | if (<表达式>) <语句> else <语句>
           | while (<表达式>) <语句>
           | continue; | break;
           | <空>;
*/
Node *statement() {
    // 调用此函数时，外部已经读入token
    // 结束时并多读token
    CHECK_ERROR
    Node *root ;

    // TODO: 指定 type ?
    if (token_type >= IDENT && token_type <= CHAR_CONST) { // 表达式的必要条件

        root = getNode();
        root->type = SingleExpStatement;
        root->val.children[0] = expression();
        if (token_type == SEMI)return root;
        REPORT_ERROR_AND_RETURN
    }

    switch (token_type) {
        case RETURN:
            root = getNode();
            root->type = ReturnStatement;
            token_type = gettoken(fp);
            if (token_type >= IDENT && token_type <= CHAR_CONST) { // 表达式的必要条件
                root->val.children[0] = expression();
                // 跳出 expression() 时，token已经读取
                if (token_type == SEMI) {
                    return root;
                }
                REPORT_ERROR_AND_RETURN
            }
            // 非表达式
            REPORT_ERROR_AND_RETURN
            break;
        case IF:// 需判断为if或if-else句式
            token_type = gettoken(fp);
            if (token_type != LP) {
                REPORT_ERROR_AND_RETURN
            }
            token_type = gettoken(fp);
            if (token_type >= IDENT && token_type <= CHAR_CONST) { // 表达式的必要条件
                root = getNode();
                root->val.children[0] = expression();

                if (token_type != RP) {
                    REPORT_ERROR_AND_RETURN
                }
                token_type = gettoken(fp);
                root->val.children[1] = statement(); // 递归调用

                token_type = gettoken(fp);
                if (token_type == ELSE) {
                    root->type = IfElseStatement;

                    token_type = gettoken(fp);
                    root->val.children[2] = statement();

                    token_type = gettoken(fp); //多读一步token
                } else {
                    root->type = IfStatement;
                }
                return root;
            }
            REPORT_ERROR_AND_RETURN
            break;
        case LCP: // {<复合语句序列>}
            token_type = gettoken(fp);
            root = compoundSentenceList();
            return root;

        default:
            REPORT_ERROR_AND_RETURN
    }
    return nullptr;
}

/*
 * <表达式> ::= <表达式> + <表达式>  |  <表达式> - <表达式> |<表达式> * <表达式>
              |<表达式> / <表达式>  | INT_CONST | IDENT | IDENT(<实参序列>)
			  |<表达式> == <表达式> |<表达式> != <表达式> |<表达式> > <表达式>
			  |<表达式>  > <表达式> |<表达式> >= <表达式> |<表达式>  <  <表达式>
			  |<表达式>  <=  <表达式>  | 标识符 = <表达式>

 */
// 表达式有两种结尾方法 一种以;结尾,一种以)结尾
Node *expression() {
    // 调用前以确保读取的第一个token为表达式必要条件---不需要?
    // 以 ; 或 ) 判断结束
    if (!(token_type >= IDENT && token_type <= CHAR_CONST) ){ // 表达式的必要条件不满足
        REPORT_ERROR_AND_RETURN
    }

    return nullptr;
}


// <局部变量定义> ::= <类型说明符> <变量序列>;
Node *localVarDef() {
    // 函数进入前提: token_type 为 <类型说明符>
    CHECK_ERROR
    Node *root = getNode();
    root->type = LocalVarDef;
    root->val.children[0] = getNode();
    root->val.children[0]->type = token_type;

    // 开始也交由 varList()判断
    root->val.children[1] = varList_up(); // 意为读取判断控制权交由 varList()
    // 结束交由 varList()判断
    return root;
}

// <变量序列> ::= <变量> , <变量序列>  |  <变量>
Node *varList_up() {
    CHECK_ERROR
    token_type = gettoken(fp);
    if (token_type != IDENT) {
        REPORT_ERROR_AND_RETURN
        return nullptr;
    }
    Node *root = getNode();
    root->type = VarList;
    Node *lc = getNode();
    lc->type = IDENT;
    strcpy(lc->val.text, token_text);
    root->val.children[0] = lc;

    // 判断是否应当结束
    token_type = gettoken(fp);
    if (token_type == SEMI) { // ;
        root->val.children[2] = nullptr;
        return root;
    }
    if (token_type == COMMA) { // ,
        root->val.children[2] = varList_up();
        return root;
    }
    // 非 ; 或 ,
    REPORT_ERROR_AND_RETURN
    return nullptr;
}


Node *formalParaList() {
    CHECK_ERROR
    if (token_type == RP) {
        return nullptr;
    }
    if (token_type < INT || token_type > CHAR) {
        REPORT_ERROR_AND_RETURN
    }

    Node *root = getNode();
    root->type = FormalParaList;

    root->val.children[0] = formalPara();

    token_type = gettoken(fp);
    if (token_type == COMMA) {
        token_type = gettoken(fp);
        if (token_type < INT || token_type > CHAR) { // 确保 etc. int func(int a,) 不合法
            REPORT_ERROR_AND_RETURN
        }
        root->val.children[1] = formalParaList();
        return root;
    }
    if (token_type == LP) {
        root->val.children[1] = nullptr;
        return root;
    }


    return nullptr;
}

Node *formalPara() {
    CHECK_ERROR
    tmp_type = token_type;

    token_type = gettoken(fp);
    if (token_type != IDENT) {
        REPORT_ERROR_AND_RETURN
    }
    Node *root = getNode();
    root->type = FormalPara;

    Node *lc = getNode();
    lc->type = tmp_type;
    Node *rc = getNode();
    rc->type = IDENT;
    strcpy(rc->val.text, token_text);

    root->val.children[0] = lc;
    root->val.children[1] = rc;

    return root;
}
