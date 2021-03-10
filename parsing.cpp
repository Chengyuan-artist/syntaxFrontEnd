//
// Created by 张承元 on 2021/3/2.
//

#include "syntax.hpp"
#include <stack>

#define CHECK_ERROR if(error)return nullptr;
#define REPORT_ERROR_AND_RETURN error = 1;return nullptr;

int error = 0;
int error_type;

extern char token_text[20];
extern int then_row; // 当前行数

FILE *fp;// init in main
token_kind token_type; //value from gettoken

// tmp var to save the last res form gettoken
token_kind tmp_type;
char tmp_token[20];

#define OUT -2
int op_precede_sheet[10][10] =
        {
                {1,  -1, 1,     OUT, 1,  1,  1,  1,  1,     1,},
                {1,  1,  1,     OUT, 1,  1,  1,  1,  1,     1,},
                {1,  1,  1,     OUT, 1,  1,  1,  1,  1,     1,},
                {-1, -1, 0,     OUT, -1, -1, -1, -1, -1, OUT,},
                {OUT, OUT, OUT, OUT, 1, OUT, OUT, OUT, OUT, 1,},
                {-1, -1, 1,     OUT, 1,  1,  1,  1,  1,     1,},
                {-1, -1, 1,     OUT, 1,  -1, 1,  1,  1,     1,},
                {-1, -1, 1,     OUT, 1,  -1, -1, 1,  1,     1,},
                {-1, -1, 1,     OUT, 1,  -1, -1, -1, 1,     1,},
                {-1, -1, -1,    OUT, -1, -1, -1, -1, -1,    0}
        };

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

Node *getNode() {
    Node *root = (Node *) malloc(sizeof(Node));
    root->type = NoType;
    for (int i = 0; i < 4; ++i) {
        root->val.children[i] = nullptr;
    }
    return root;
}


int precede(int op, int last_op);

void visit(Node *root);

void visit(Node *root) { // 递归写法

    // TODO: unfinished
    switch (root->type) {
        case Program:
            printf("Program:\n");
            break;
        case ExtDefList:
            break;
        case ExtVarDef:
            printf("\tExtVarDef:\n");
            break;
        case FuncDef:
            printf("\tFuncDef:\n");
            break;

    }

}

Node *program(FILE *in) {
    fp = in;

    Node *root = getNode();
    root->type = Program;

    token_type = gettoken(fp);
    root->val.children[0] = extDefList();

    // TODO 语法错误传递

    return root;
}

Node *extDefList() {
    CHECK_ERROR
    if (token_type == Eof) return nullptr;

    Node *root = getNode();
    root->type = ExtDefList;
    root->val.children[0] = extDef();

    token_type = gettoken(fp);
    root->val.children[1] = extDefList();
    return root;
}

Node *extDef() {
    if (token_type < INT || token_type > CHAR) {
        REPORT_ERROR_AND_RETURN
    }
    tmp_type = token_type;

    token_type = gettoken(fp);
    if (token_type != Identifier) {
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
    root->type = DeclaratorList;

    // 左孩子
    Node *lc = getNode();
    lc->type = Identifier;
    strcpy(lc->val.text, tmp_token);
    root->val.children[0] = lc;


    if (token_type == COMMA) {
        token_type = gettoken(fp);
        if (token_type != Identifier) {
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

    Node *name = getNode();
    name->type = Identifier;
    strcpy(name->val.text, token_text);
    root->val.children[1] = name;

    token_type = gettoken(fp);
    root->val.children[2] = formalParaList();

    token_type = gettoken(fp);
    if (token_type == SEMI) {
        root->val.children[3] = nullptr;
        return root;
    }
    if (token_type != LCP) {
        REPORT_ERROR_AND_RETURN
    }
    // token_type == {
    token_type = gettoken(fp);
    root->val.children[3] = compoundSentenceList();

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
    if (token_type >= Identifier && token_type <= CHAR_CONST) { // 表达式的必要条件

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
            if (token_type >= Identifier && token_type <= CHAR_CONST) { // 表达式的必要条件
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
            if (token_type >= Identifier && token_type <= CHAR_CONST) { // 表达式的必要条件
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
    // 表达式有三种类型的单体
    // 常量 变量 函数调用  etc. 12.3 + a + func(5, b);
    if (!(token_type >= Identifier && token_type <= CHAR_CONST)) { // 表达式的必要条件不满足
        REPORT_ERROR_AND_RETURN
    }

    // TODO: 先不考虑 fuc(5,b)
    // 非递归，而是设栈
    std::stack<Node *> opt;
    std::stack<Node *> opn;

    Node *end_mark = getNode();
    end_mark->type = Begin_Op;

    opt.push(end_mark);

    while ((token_type != Begin_Op) && !error) {

        if (token_type >= Identifier && token_type <= CHAR_CONST) { // 如果token是操作数
            // TODO: 函数调用形式
            Node *num = getNode();
            num->type = token_type;
            strcpy(num->val.text, token_text);

            opn.push(num);

            token_type = gettoken(fp);
        } else {
            if (token_type >= ASSIGN && token_type <= Ige) { // token是运算符
                Node *then_opt = getNode();
                then_opt->type = token_type;

                Node *last_opt = opt.top();
                switch (precede(token_type, last_opt->type)) {
                    case 1:
                        opt.push(then_opt);
                        token_type = gettoken(fp);
                        break;
                    case 0:
                        opt.pop();
                        if (opn.empty()) {
                            error = 1;
                            break;
                        } else {
                            Node *opn_bracket = getNode();
                            opn_bracket->type = Brackets;
                            opn_bracket->val.children[0] = opn.top();

                            opn.pop();
                            opn.push(opn_bracket);
                            token_type = gettoken(fp);
                        }
                        break;
                    case -1:
                        if (opn.size() < 2) {
                            error = 1;
                            break;
                        } else {
                            Node *right_opn = opn.top();
                            opn.pop();
                            Node *left_opn = opn.top();
                            opn.pop();

                            opt.pop();
                            last_opt->val.children[0] = left_opn;
                            last_opt->val.children[1] = right_opn;

                            opn.push(last_opt);
                        }
                        break;
                    default:
                        if (token_type == SEMI || token_type == RP) break;
                        else error = 1;
                }
            } else {
                if (token_type == SEMI || token_type == RP) break;
                else error = 1;
            }

        }
    }

    if (!error && opn.size() == 1 && opt.top()->type == Begin_Op && opt.size() == 1) {
        // 运算符和操作数相匹配
        return opn.top();
    } else {
        return nullptr;
    }
}


int op_priority(int stdType) {
    int a = stdType;
    if (a == PLUS || a == MINUS) return 0;
    if (a == MULTIPLY || a == DIVIDE) return 1;
    if (a == LP) return 2;
    if (a == RP) return 3;
    if (a == ASSIGN) return 4;
    if (a == Igt || a == Ige || a == Clt || a == Cle) return 5;
    if (a == EQ || a == UEQ) return 6;
    if (a == AndAnd) return 7;
    if (a == OrOr) return 8;
    if (a == Begin_Op) return 9;

    error = 1;
    return -1;
}

int precede(int op, int last_op) {
    int _op_p = op_priority(op);
    int _last_op_p = op_priority(last_op);
    if (_op_p != -1 && _last_op_p != -1) {
        return op_precede_sheet[_op_p][_last_op_p];
    }
    error = 1;
    return OUT;
}


// <局部变量定义> ::= <类型说明符> <变量序列>;
Node *localVarDef() {
    // 函数进入前提: token_type 为 <类型说明符>
    CHECK_ERROR
    Node *root = getNode();
    root->type = LocalVarDef;
    root->val.children[0] = getNode();
    root->val.children[0]->type = token_type;

    // 开始也交由 declaratorList()判断
    root->val.children[1] = varList_up(); // 意为读取判断控制权交由 declaratorList()
    // 结束交由 declaratorList()判断
    return root;
}

// <变量序列> ::= <变量> , <变量序列>  |  <变量>
Node *varList_up() {
    CHECK_ERROR
    token_type = gettoken(fp);
    if (token_type != Identifier) {
        REPORT_ERROR_AND_RETURN
        return nullptr;
    }
    Node *root = getNode();
    root->type = DeclaratorList;
    Node *lc = getNode();
    lc->type = Identifier;
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
    if (token_type != Identifier) {
        REPORT_ERROR_AND_RETURN
    }
    Node *root = getNode();
    root->type = FormalPara;

    Node *lc = getNode();
    lc->type = tmp_type;
    Node *rc = getNode();
    rc->type = Identifier;
    strcpy(rc->val.text, token_text);

    root->val.children[0] = lc;
    root->val.children[1] = rc;

    return root;
}
