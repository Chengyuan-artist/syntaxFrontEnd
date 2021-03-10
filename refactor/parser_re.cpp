//
// Created by 张承元 on 2021/3/8.
//


#include "parser_re.hpp"

#define REPORT_ERROR_AND_RETURN parser->error = 1;return nullptr;
#define CHECK_ERROR if(parser->error)return nullptr;


Parser *GetParser(FILE *fp){
    Parser *parser = (Parser *)malloc(sizeof(Parser));
    memset(parser, 0 , sizeof(Parser));
    parser->in = fp;
    return parser;
}

void Parsing(Parser *parser){
    if (parser->token_list == nullptr) return;
    parser->root = program(parser);
}

void LexicalAnalyse(Parser *parser) {
    parser->token_list = GetTokenList(parser->in);

    // token error catch
}

Node *program(Parser *parser) {
    Node *root = GetNode(Program);

    AddChild(root, extDefList(parser));

    if (parser->error) {
        DeleteNode(root);
        root = nullptr;
    }

    // TODO 语法错误传递
    return root;
}

Node *extDefList(Parser *parser) {
    CHECK_ERROR
    Token *token = CurrentToken(parser->token_list);
    if (token->type == Eof) {
        return nullptr;
    }
    Node *root = GetNode(ExtDefList);

    AddChild(root, extDef(parser));

    AddChild(root, extDefList(parser));
    return root;
}

Node *extDef(Parser *parser) {
    CHECK_ERROR
    // 判断区分变量定义与函数定义

    Token *token = CurrentToken(parser->token_list);

    // 如果 token 不是类型说明符
    if (token->type < INT || token->type > CHAR) {
        REPORT_ERROR_AND_RETURN
    }

    token = NextToken(parser->token_list);

    if (token->type != Identifier) {
        REPORT_ERROR_AND_RETURN;
    }

    token = NextToken(parser->token_list);

    if (token->type == LP) {
        return funcDef(parser);
    } else return extVarDef(parser);

}

Node *extVarDef(Parser *parser) {
    CHECK_ERROR
    Node *root = GetNode(ExtVarDef);

    Node *type_id = GetNode(TokenAt(parser->token_list, -2));

    AddChild(root, type_id);

    AddChild(root, declaratorList(parser));

    return root;
}

// <函数定义>：：=<类型说明符> <函数名> (<形式参数序列>) {<复合语句序列>}  (可无)
Node *funcDef(Parser *parser) {
    CHECK_ERROR

    Node *root = GetNode(FuncDef);

    Node *type_id = GetNode(TokenAt(parser->token_list, -2));

    AddChild(root, type_id);

    Node *func_name = GetNode(TokenAt(parser->token_list, -1));

    AddChild(root, func_name);

    NextToken(parser->token_list);
    AddChild(root, formalParaList(parser));


    Token *token = NextToken(parser->token_list);

    if (token->type == SEMI) {
        return root;
    }

    // 花括号交由func判断
    if (token->type != LCP) {
        DeleteNode(root);
        REPORT_ERROR_AND_RETURN
    }

    Node *curly_braces = GetNode(CurlyBraces);

    NextToken(parser->token_list);
    AddChild(curly_braces, compoundStatementList(parser));

    AddChild(root, curly_braces);

    return root;
}

Node *declaratorList(Parser *parser) {
    CHECK_ERROR

    Node *root = GetNode(DeclaratorList);
    // 左孩子
    Node *var = declarator(parser);
    AddChild(root, var);

    Token *token = CurrentToken(parser->token_list);
    if (token->type == COMMA) {
        token = NextToken(parser->token_list);
        if (token->type != Identifier) {
            // error
            DeleteNode(root);
            REPORT_ERROR_AND_RETURN
        }

        NextToken(parser->token_list);
        AddChild(root, declaratorList(parser)); // 递归调用
        return root;
    }
    if (token->type == SEMI) {
        NextToken(parser->token_list);
        return root;
    }

    REPORT_ERROR_AND_RETURN
}

Node *declarator(Parser *parser) {
    CHECK_ERROR
    // 调用前token指针指向示意
    // int id [ 10 ]      或     int  id , a , b [ 4 ]
    //        ^                          ^
    Node *root = GetNode();

    Node *identifier = GetNode(TokenAt(parser->token_list, -1));

    Token *token = CurrentToken(parser->token_list);
    if (token->type == LSP) { // 如果是左方括号
        root->type = ArrayDeclarator;
        AddChild(root, identifier);

        // 以下按照 正确条件书写
        token = NextToken(parser->token_list);
        if (token->type == INT_CONST || token->type == LONG_CONST) {
            Node *constant = GetNode(token);
            AddChild(root, constant);

            token = NextToken(parser->token_list);
            if (token->type == RSP) {
                // 消耗掉一个 ]
                NextToken(parser->token_list);
                return root;
            } else {
                DeleteNode(root);
                REPORT_ERROR_AND_RETURN
            }
        } else {
            DeleteNode(root);
            REPORT_ERROR_AND_RETURN
        }
    }

    // 如果不是左方括号
    DeleteNode(root);

    return identifier;
    // 调用后token指针指向示意
    // int id [ 10 ] ;     或     int  id , a , b [ 4 ]
    //               ^                    ^
}


// <形式参数序列>：：=<形式参数> , <形式参数序列>  |  <空>
Node *formalParaList(Parser *parser) {
    CHECK_ERROR

    Token *token = CurrentToken(parser->token_list);
    if (token->type == RP) {
        return nullptr;
    }

    Node *root = GetNode(FormalParaList);

    AddChild(root, formalPara(parser));

    token = NextToken(parser->token_list);
    if (token->type == COMMA) {
        token = NextToken(parser->token_list);

        if (token->type < INT || token->type > CHAR) {
            // 确保 etc. int func(int a,) 不合法
            REPORT_ERROR_AND_RETURN
        }

        AddChild(root, formalParaList(parser));
        return root;
    }
    if (token->type == RP) {
        return root;
    }

    DeleteNode(root);
    REPORT_ERROR_AND_RETURN
}

// <形式参数>：：=<类型说明符> 标识符
Node *formalPara(Parser *parser) {
    CHECK_ERROR
    Token *token = CurrentToken(parser->token_list);
    // 如果不是类型说明符
    if (token->type < INT || token->type > CHAR) {
        REPORT_ERROR_AND_RETURN
    }
    Node *root = GetNode(FormalPara);


    Node *lc = GetNode( CurrentToken(parser->token_list) );

    AddChild(root, lc);

    // need an identity
    token = NextToken(parser->token_list);
    if (token->type != Identifier) {
        DeleteNode(root);
        REPORT_ERROR_AND_RETURN
    }

    Node *rc = GetNode(CurrentToken(parser->token_list));

    AddChild(root, rc);

    return root;
}

Node *compoundStatementList(Parser *parser) {
    CHECK_ERROR
    Token *token = CurrentToken(parser->token_list);

    if (token->type == RCP) {
        NextToken(parser->token_list); // 消耗掉右花括号
        return nullptr;
    }

    Node *root = GetNode(CompoundSentenceList);

    AddChild(root, compoundStatement(parser));

    // need to forward? yes
    // 退出compoundStatement() token指针会停留在Statement的结束符上
    // 不 已更改Statement,指针会更进一步

//    NextToken(parser->token_list);
    AddChild(root, compoundStatementList(parser));

    return root;
}

// <复合语句> ::= <局部变量定义>  |  <语句>
Node *compoundStatement(Parser *parser) {
    CHECK_ERROR
    Token *token = CurrentToken(parser->token_list);
    if (token->type >= INT && token->type <= CHAR) {
        // 是 <类型说明符>
        return localVarDef(parser);
    } else {
        return statement(parser);
    }
}

// <局部变量定义> ::= <类型说明符> <变量序列>;
Node *localVarDef(Parser *parser) {
    CHECK_ERROR

    Node *root = GetNode(LocalVarDef);

    Node *type_id = GetNode(CurrentToken(parser->token_list));

    AddChild(root, type_id);

    Token *token = NextToken(parser->token_list);

    if (token->type != Identifier){
        REPORT_ERROR_AND_RETURN
    }
    NextToken(parser->token_list);

    AddChild(root, declaratorList(parser));

    return root;
}


Node *statement(Parser *parser) {
    CHECK_ERROR
    Node *root = GetNode();

    Token *token = CurrentToken(parser->token_list);

    switch (token->type) {
        case RETURN:
            root->type = ReturnStatement;
            NextToken(parser->token_list);
            AddChild(root, expression(parser));
            // 检查分号
            token = CurrentToken(parser->token_list);
            if (token->type != SEMI) {
                DeleteNode(root);
                REPORT_ERROR_AND_RETURN
            }
            token = NextToken(parser->token_list); // 消耗掉分号
            break;

        case IF:
            // 检查左括号
            token = NextToken(parser->token_list);
            if (token->type != LP) {
                DeleteNode(root);
                REPORT_ERROR_AND_RETURN
            }

            NextToken(parser->token_list);
            AddChild(root, expression(parser));

            // 检查右括号
            token = CurrentToken(parser->token_list);
            if (token->type != RP) {
                DeleteNode(root);
                REPORT_ERROR_AND_RETURN
            }

            NextToken(parser->token_list);
            AddChild(root, statement(parser));

            // statement的结束符已由其消灭掉
            // token指针此时停留在statement的下一个token上

            token = CurrentToken(parser->token_list);
            if (token->type == ELSE) {
                root->type = IfElseStatement;

                NextToken(parser->token_list);
                AddChild(root, statement(parser));
                break;
            } else {
                root->type = IfStatement;
            }
            break;

        case WHILE:
            root->type = WhileStatement;

            // 检查左括号
            token = NextToken(parser->token_list);
            if (token->type != LP) {
                DeleteNode(root);
                REPORT_ERROR_AND_RETURN
            }

            NextToken(parser->token_list);
            AddChild(root, expression(parser));

            // 检查右括号
            token = CurrentToken(parser->token_list);
            if (token->type != RP) {
                DeleteNode(root);
                REPORT_ERROR_AND_RETURN
            }

            NextToken(parser->token_list);
            AddChild(root, statement(parser));
            break;

        case FOR:
            root->type = ForStatement;
            // 检查左括号
            token = NextToken(parser->token_list);
            if (token->type != LP) {
                DeleteNode(root);
                REPORT_ERROR_AND_RETURN
            }

            // 检查for前两个条件及分号
            for (int i = 0; i < 2; ++i) {
                token = NextToken(parser->token_list);
                if (token->type == SEMI) {
                    AddChild(root, nullptr);
                }else {
                    AddChild(root, expression(parser));
                    token = CurrentToken(parser->token_list);
                    if (token->type == SEMI){
                        continue;
                    } else{
                        DeleteNode(root);
                        REPORT_ERROR_AND_RETURN
                    }
                }
            }

            // 消耗掉第二个条件的分号
            // 检查第三个条件
            token = NextToken(parser->token_list);
            if (token->type == SEMI) {
                AddChild(root, nullptr);
            }else {
                AddChild(root, expression(parser));
            }

            // 检查右括号
            token = CurrentToken(parser->token_list);
            if (token->type != RP) {
                DeleteNode(root);
                REPORT_ERROR_AND_RETURN
            }

            NextToken(parser->token_list);
            AddChild(root, statement(parser));
            break;


        case BREAK:
            root->type = BreakStatement;
            token = NextToken(parser->token_list);
            if (token->type != SEMI) {
                REPORT_ERROR_AND_RETURN
            }
            NextToken(parser->token_list);
            break;

        case CONTINUE:
            root->type = ContinueStatement;
            token = NextToken(parser->token_list);
            if (token->type != SEMI) {
                REPORT_ERROR_AND_RETURN
            }
            NextToken(parser->token_list);
            break;

        case LCP:
            root->type = CurlyBraces;
            NextToken(parser->token_list);
            AddChild(root, compoundStatementList(parser));
            // 从compoundStatementList() 退出时，
            // token指针应停留在 } 后

            break;

        default:
            root->type = SingleExpStatement;
            AddChild(root, expression(parser));
            // 检查分号
            token = CurrentToken(parser->token_list);
            if (token->type != SEMI) {
                DeleteNode(root);
                REPORT_ERROR_AND_RETURN
            }
            NextToken(parser->token_list);
            break;
    }

    // 正常退出时,token指针应停留在  ;之后  或 }后

    return root;

}

#include <stack>

using namespace std;

#define Set_Max_Size 40
#define Exp_Legal_Token_Set_Len 20
#define Operator_Set_Len 14

enum TokenType exp_legal_token_set[Set_Max_Size] =
        {
                ASSIGN, LP, AndAnd, OrOr, PLUS, MINUS, MULTIPLY,
                DIVIDE, MOD, EQ, UEQ, Clt, Cle, Igt, Ige, Identifier,
                INT_CONST, LONG_CONST, FLOAT_CONST, CHAR_CONST,
        };

enum TokenType operator_set[Set_Max_Size] =
        {
                ASSIGN, AndAnd, OrOr, PLUS, MINUS, MULTIPLY,
                DIVIDE, MOD, EQ, UEQ, Clt, Cle, Igt, Ige,
        };

int isElemInSet(enum TokenType elem, const enum TokenType *set, int len) {
    for (int i = 0; i < len; ++i) {
        if (elem == set[i]) return 1;
    }
    return 0;
}

Node *argumentsList(Parser *parser) {
    CHECK_ERROR
    Token *token = CurrentToken(parser->token_list);

    if (token->type == RP) {
        return nullptr;
    }

    Node *root = GetNode(ArgumentsList);
    AddChild(root, expression(parser));

    token = CurrentToken(parser->token_list);

    if (token->type == COMMA) {
        NextToken(parser->token_list);
        AddChild(root, argumentsList(parser));
    }

    return root;
}

Node *expression(Parser *parser) {
    CHECK_ERROR

    Token *token = CurrentToken(parser->token_list);

    int need_operand = 1;

    stack<Node *> opRand;
    stack<Token *> opT;

    Token *begin_op = GetToken();
    begin_op->type = Begin_Op;
    opT.push(begin_op);

    // 判断条件 属于expression的合法字符
    while (isElemInSet(token->type, exp_legal_token_set, Exp_Legal_Token_Set_Len)) {

        // 若token为操作符
        if (isElemInSet(token->type, operator_set, Operator_Set_Len)) {
            if (need_operand) {
                REPORT_ERROR_AND_RETURN
            }

            Token *last_op = opT.top();

            if (precede(token->type, last_op->type)) { // 运算符优先级高于栈顶运算符
                opT.push(token);
                token = NextToken(parser->token_list);
                need_operand = 1;

            } else { // 运算符优先级低于栈顶运算符
                Node *r_operand = opRand.top();
                opRand.pop();
                Node *l_operand = opRand.top();
                opRand.pop();

                Node *new_operand = GetNode(last_op);
                opT.pop();

                AddChild(new_operand, l_operand);
                AddChild(new_operand, r_operand);

                if (token->type == ASSIGN) {
                    if ((l_operand->type == TokenType && l_operand->token->type == Identifier)
                        || (l_operand->type == Array)) {
                        // 可赋值左值 合法
                    }else {
                        DeleteNode(new_operand);
                        REPORT_ERROR_AND_RETURN
                    }
                }

                // 不读入新的
                opRand.push(new_operand);
                need_operand = 0;
            }



        } else { // token为合法的其他字符->对应操作数

            if (!need_operand) {
                REPORT_ERROR_AND_RETURN
            }

            if (token->type == LP) {
                // 将括号内的内容视作一个操作数
                NextToken(parser->token_list); // 消耗掉左括号

                Node *new_operand = GetNode(Parentheses);
                AddChild(new_operand, expression(parser));

                token = CurrentToken(parser->token_list);
                if (token->type != RP) {
                    // 括号不匹配
                    REPORT_ERROR_AND_RETURN
                }

                opRand.push(new_operand); // 新操作数进栈

                token = NextToken(parser->token_list); // 消耗掉右括号

            } else {
                // 操作数对应四种类型: identifier, a[<expression>], func(c,d...)
                //                  constant

                if (token->type == Identifier) {
                    // 从三种类型中判断

                    Node *new_operand = GetNode();
                    Node *operand_type = GetNode(token);

                    token = NextToken(parser->token_list);

                    switch (token->type) {
                        case LP:
                            new_operand->type = Function;
                            AddChild(new_operand, operand_type);
                            NextToken(parser->token_list);
                            AddChild(new_operand, argumentsList(parser));

                            // 检查右括号
                            token = CurrentToken(parser->token_list);
                            if (token->type != RP) {
                                REPORT_ERROR_AND_RETURN
                            }
                            token = NextToken(parser->token_list); // 消耗右括号
                            break;

                        case LSP:
                            new_operand->type = Array;
                            AddChild(new_operand, operand_type);
                            NextToken(parser->token_list);
                            AddChild(new_operand, expression(parser));

                            token = CurrentToken(parser->token_list);
                            if (token->type != RSP) {
                                REPORT_ERROR_AND_RETURN
                            }
                            token = NextToken(parser->token_list); // 消耗右方括号
                            break;

                        default:
                            DeleteNode(new_operand);
                            new_operand = operand_type;
                            break;
                    }

                    opRand.push(new_operand);

                } else { // constant
                    Node *new_operand = GetNode(token);
                    opRand.push(new_operand);
                    token = NextToken(parser->token_list);
                }
            }
            need_operand = 0;
        }
    }

    CHECK_ERROR

    if (need_operand) {
        REPORT_ERROR_AND_RETURN
    }

    while (opT.size() != 1) {
        Token *last_op = opT.top();
        Node *new_operand = GetNode(last_op);
        opT.pop();

        Node *r_operand = opRand.top();
        opRand.pop();
        Node *l_operand = opRand.top();
        opRand.pop();

        AddChild(new_operand, l_operand);
        AddChild(new_operand, r_operand);

        opRand.push(new_operand);
    }

    Node *root = GetNode(Expression);
    AddChild(root, opRand.top());

    return root;
}


int precede(enum TokenType op1, enum TokenType op2) {
    if (priority(op1) < priority(op2)) {
        return 1;
    }
    return 0;
}

int priority(enum TokenType type) {

    enum TokenType a = type;
    if (a == MULTIPLY || a == DIVIDE || a == MOD) return 3;
    if (a == PLUS || a == MINUS) return 4;
    if (a == Igt || a == Ige || a == Clt || a == Cle) return 6;
    if (a == EQ || a == UEQ) return 7;
    if (a == AndAnd) return 11;
    if (a == OrOr) return 12;
    if (a == ASSIGN) return 14;
    if (a == Begin_Op)return 15;

    return 16; // Max
}
