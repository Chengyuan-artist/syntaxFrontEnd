//
// Created by 张承元 on 2021/3/8.
//


#include "parser_re.hpp"

#define REPORT_ERROR_AND_RETURN parser->error = 1;return nullptr;
#define CHECK_ERROR if(parser->error)return nullptr;

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

char error_type_string[20][30] = {
        "Token_Error", "Need_Specifier", "Need_Identifier",
        "Need_Semi", "Brackets_Not_Match", "Type_Not_Match",
        "Form_Not_Match", "Illegal_Lvalue", "Function_Def_Not_Allowed",
        "Else_No_Match",
};

char *ToString(enum ErrorType type) {
    return error_type_string[type];
}

int isElemInSet(enum TokenType elem, const enum TokenType *set, int len) {
    for (int i = 0; i < len; ++i) {
        if (elem == set[i]) return 1;
    }
    return 0;
}


Parser *GetParser(FILE *fp) {
    Parser *parser = (Parser *) malloc(sizeof(Parser));
    memset(parser, 0, sizeof(Parser));
    parser->in = fp;
    return parser;
}

void Parsing(Parser *parser) {
    if (parser->token_list == nullptr) return;
    parser->root = program(parser);

    if (parser->error) {
        printf("%s: %s: in line %d\n", ToString(parser->error_pos),
               ToString(parser->error_type),
               parser->error_row);
    }
}

void LexicalAnalyse(Parser *parser) {
    parser->token_list = GetTokenList(parser->in);
    // token error catch

    TokenList *list = parser->token_list;
    Token *token;
    for (int i = 0; i < parser->token_list->len; ++i) {
        token = TokenAt(list, i);
        if (token->type == ERROR_TOKEN) {
            printf("%s : at line %d\n", ToString(ERROR_TOKEN), token->then_row);
            parser->error = 1;
            parser->error_type = Token_Error;
        }
    }
}

void displayAllToken(Parser *parser) {
    if (parser->token_list == nullptr)return;
    Token *token;
    for (int i = 0; i < parser->token_list->len; ++i) {
        token = TokenAt(parser->token_list, i);
        printf("%d: %s\n", i + 1, ToString(token->type));
        if (token->type == Identifier
            || token->type == INT_CONST
            || token->type == FLOAT_CONST
            || token->type == LONG_CONST
            || token->type == CHAR_CONST
            || token->type == INCLUDE
            || token->type == DEFINE) {
            printf("\t%s\n", token->text);
        }
    }
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

        RecordError(parser, token->then_row, ExtDef, Need_Specifier);
        REPORT_ERROR_AND_RETURN
    }

    token = NextToken(parser->token_list);

    if (token->type != Identifier) {

        RecordError(parser, token->then_row, ExtDef, Need_Identifier);
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
        NextToken(parser->token_list);
        return root;
    }

    // 花括号交由func判断
    if (token->type != LCP) {
        DeleteNode(root);
        RecordError(parser, token->then_row, FuncDef, Need_Semi);
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

            RecordError(parser, token->then_row, DeclaratorList, Need_Specifier);
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
    if (token->type == LP) {
        DeleteNode(root);
        RecordError(parser, token->then_row, DeclaratorList, Function_Def_Not_Allowed);
        REPORT_ERROR_AND_RETURN
    }

    DeleteNode(root);
    RecordError(parser, token->then_row, DeclaratorList, Need_Semi);
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
                RecordError(parser, token->then_row, ArrayDeclarator, Brackets_Not_Match);
                REPORT_ERROR_AND_RETURN
            }
        } else {
            DeleteNode(root);
            RecordError(parser, token->then_row, ArrayDeclarator, Type_Not_Match);
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
            RecordError(parser, token->then_row, FormalParaList, Form_Not_Match);
            REPORT_ERROR_AND_RETURN
        }

        AddChild(root, formalParaList(parser));
        return root;
    }
    if (token->type == RP) {
        return root;
    }

    DeleteNode(root);
    RecordError(parser, token->then_row, ArrayDeclarator, Brackets_Not_Match);
    REPORT_ERROR_AND_RETURN
}

// <形式参数>：：=<类型说明符> 标识符
Node *formalPara(Parser *parser) {
    CHECK_ERROR
    Token *token = CurrentToken(parser->token_list);
    // 如果不是类型说明符
    if (token->type < INT || token->type > CHAR) {
        RecordError(parser, token->then_row, FormalPara, Need_Specifier);
        REPORT_ERROR_AND_RETURN
    }
    Node *root = GetNode(FormalPara);


    Node *lc = GetNode( CurrentToken(parser->token_list) );

    AddChild(root, lc);

    // need an identity
    token = NextToken(parser->token_list);
    if (token->type != Identifier) {
        DeleteNode(root);
        RecordError(parser, token->then_row, FormalPara, Need_Identifier);
        REPORT_ERROR_AND_RETURN
    }

    Node *rc = GetNode(CurrentToken(parser->token_list));

    AddChild(root, rc);

    return root;
}

Node *compoundStatementList(Parser *parser) {
    CHECK_ERROR
    Token *token = CurrentToken(parser->token_list);

    if (token->type == Eof) {
        RecordError(parser, token->then_row, CompoundSentenceList, Brackets_Not_Match);
        REPORT_ERROR_AND_RETURN
    }

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
        RecordError(parser, token->then_row, LocalVarDef, Need_Identifier);
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
                RecordError(parser, token->then_row, ReturnStatement, Need_Semi);
                REPORT_ERROR_AND_RETURN
            }
            token = NextToken(parser->token_list); // 消耗掉分号
            break;

        case IF:
            // 检查左括号
            token = NextToken(parser->token_list);
            if (token->type != LP) {
                DeleteNode(root);
                RecordError(parser, token->then_row, IfStatement, Form_Not_Match);
                REPORT_ERROR_AND_RETURN
            }

            NextToken(parser->token_list);
            AddChild(root, expression(parser));

            // 检查右括号
            token = CurrentToken(parser->token_list);
            if (token->type != RP) {
                DeleteNode(root);
                RecordError(parser, token->then_row, IfStatement, Brackets_Not_Match);
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
                RecordError(parser, token->then_row, WhileStatement, Form_Not_Match);
                REPORT_ERROR_AND_RETURN
            }

            NextToken(parser->token_list);
            AddChild(root, expression(parser));

            // 检查右括号
            token = CurrentToken(parser->token_list);
            if (token->type != RP) {
                DeleteNode(root);
                RecordError(parser, token->then_row, WhileStatement, Brackets_Not_Match);
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
                RecordError(parser, token->then_row, ForStatement, Form_Not_Match);
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
                        RecordError(parser, token->then_row, ForStatement, Need_Semi);
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
                RecordError(parser, token->then_row, ForStatement, Brackets_Not_Match);
                REPORT_ERROR_AND_RETURN
            }

            NextToken(parser->token_list);
            AddChild(root, statement(parser));
            break;


        case BREAK:
            root->type = BreakStatement;
            token = NextToken(parser->token_list);
            if (token->type != SEMI) {
                RecordError(parser, token->then_row, BreakStatement, Need_Semi);
                REPORT_ERROR_AND_RETURN
            }
            NextToken(parser->token_list);
            break;

        case CONTINUE:
            root->type = ContinueStatement;
            token = NextToken(parser->token_list);
            if (token->type != SEMI) {
                RecordError(parser, token->then_row, ContinueStatement, Need_Semi);
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

        case ELSE:
            DeleteNode(root);
            RecordError(parser, token->then_row, IfElseStatement, Else_No_Match);
            REPORT_ERROR_AND_RETURN

        default:
            root->type = SingleExpStatement;
            AddChild(root, expression(parser));
            // 检查分号
            token = CurrentToken(parser->token_list);
            if (token->type != SEMI) {
                DeleteNode(root);
                RecordError(parser, token->then_row, SingleExpStatement, Need_Semi);
                REPORT_ERROR_AND_RETURN
            }
            NextToken(parser->token_list);
            break;
    }

    // 正常退出时,token指针应停留在  ;之后  或 }后

    return root;

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
        token = NextToken(parser->token_list);
        if (token->type != Identifier) {
            // 确保(a,b,)不合法
            RecordError(parser, token->then_row, ArgumentsList, Form_Not_Match);
            REPORT_ERROR_AND_RETURN
        }
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
                RecordError(parser, token->then_row, Expression, Form_Not_Match);
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
                        || (l_operand->type == ArrayCall)
                        || (l_operand->type == TokenType && l_operand->token->type == ASSIGN)) {
                        // 可赋值左值 合法
                    } else {
                        DeleteNode(new_operand);
                        RecordError(parser, token->then_row, Expression, Illegal_Lvalue);
                        REPORT_ERROR_AND_RETURN
                    }
                }

                // 不读入新的
                opRand.push(new_operand);
                need_operand = 0;
            }



        } else { // token为合法的其他字符->对应操作数

            if (!need_operand) {
                RecordError(parser, token->then_row, Expression, Form_Not_Match);
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
                    RecordError(parser, token->then_row, Expression, Brackets_Not_Match);
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
                            new_operand->type = FunctionCall;
                            AddChild(new_operand, operand_type);
                            NextToken(parser->token_list);
                            AddChild(new_operand, argumentsList(parser));

                            // 检查右括号
                            token = CurrentToken(parser->token_list);
                            if (token->type != RP) {
                                RecordError(parser, token->then_row, FunctionCall, Brackets_Not_Match);
                                REPORT_ERROR_AND_RETURN
                            }
                            token = NextToken(parser->token_list); // 消耗右括号
                            break;

                        case LSP:
                            new_operand->type = ArrayCall;
                            AddChild(new_operand, operand_type);
                            NextToken(parser->token_list);
                            AddChild(new_operand, expression(parser));

                            token = CurrentToken(parser->token_list);
                            if (token->type != RSP) {
                                RecordError(parser, token->then_row, ArrayCall, Brackets_Not_Match);
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
        RecordError(parser, token->then_row, Expression, Form_Not_Match);
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

        if (last_op->type == ASSIGN) {
            if ((l_operand->type == TokenType && l_operand->token->type == Identifier)
                || (l_operand->type == ArrayCall)
                || (l_operand->type == TokenType && l_operand->token->type == ASSIGN)) {
                // 可赋值左值 合法
            } else {
                DeleteNode(new_operand);
                RecordError(parser, token->then_row, Expression, Illegal_Lvalue);
                REPORT_ERROR_AND_RETURN
            }
        }

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

void visit_expression(Node *root) {
    if (root == nullptr)return;
    if (root->type == Expression) {
        visit_expression(root->children[0]);
        return;
    }
    if (root->type == Parentheses) {
        printf("%s", ToString(LP));
        visit_expression(root->children[0]);
        printf("%s", ToString(RP));
        return;
    }
    if (root->type == ArgumentsList) {
        visit_expression(root->children[0]);
        if (root->children[1] != nullptr) {
            printf(",");
            visit_expression(root->children[1]);
        }
        return; // 一定要return!
    }
    if (root->type == FunctionCall) {

        // TODO
        visit_expression(root->children[0]);
        printf("%s", ToString(LP));
        visit_expression(root->children[1]);
        printf("%s", ToString(RP));

        return; // 一定要return
    }
    if (root->type == ArrayCall) {

        visit_expression(root->children[0]);
        printf("%s", ToString(LSP));
        visit_expression(root->children[1]);
        printf("%s", ToString(RSP));
        return; // 一定要return
    }
    visit_expression(root->children[0]);
    if (root->token->type == Identifier || isConstant(root->token->type))
        printf("%s", root->token->text);
    else printf("%s", ToString(root->token->type));

    visit_expression(root->children[1]);
}

void visit(Node *root, int layer) {
    if (root == nullptr) return;
    indent(layer);
    if (root->type == TokenType) {
        printf("%s", ToString(root->token->type));
        if (root->token->type == Identifier || root->token->type == INT_CONST) {
            printf(": %s", root->token->text);
        }
        printf("\n");
    } else {
        printf("%s\n", ToString(root->type));
        if (root->type == Expression) {
            // 中序遍历
            indent(layer+1);
            visit_expression(root);
            printf("\n");
        } else {
            // 先序遍历
            for (int i = 0; i < root->child_num; ++i) {
                visit(root->children[i], layer + 1);
            }
        }
    }
}

void indent(int layer) {
    for (int i = 0; i < layer; ++i) {
        printf("%s", "~---");
    }
}

void RecordError(Parser *parser, int error_row, enum NodeType error_pos, enum ErrorType error_type) {
    parser->error = 1;
    parser->error_row = error_row;
    parser->error_pos = error_pos;
    parser->error_type = error_type;

}

void PreProcess(Parser *parser) {
    LexicalAnalyse(parser);

    if (parser->error) return;


    Token *token;
    for (int i = 0; i < parser->token_list->len; ++i) {
        token = TokenAt(parser->token_list, i);

        if (token->type == INCLUDE) {
            FILE *fin = fopen(token->text, "r");
            if (fin == nullptr) {
                printf("Include_Error at line %d : fail to open file \"%s\" ",token->then_row,token->text);
                parser->error = 1;
                parser->error_type = Token_Error;
                return;
            }

            Parser *new_parser = GetParser(fin);
            PreProcess(new_parser);

            if (new_parser->error || new_parser->token_list == nullptr) {
                parser->error = 1;
                return;
            }

            DeleteToken(parser->token_list, i);
            DeleteToken(new_parser->token_list, new_parser->token_list->len-1); // 删除Eof

            int tmp_len = new_parser->token_list->len;
            parser->token_list = InsertList(parser->token_list, new_parser->token_list, i);


            i += (tmp_len - 1 );
            free(new_parser);
            continue;
        }

        if (token->type == DEFINE) {
            DeleteToken(parser->token_list, i); // 直接删除处理
            continue;
        }
    }
}
