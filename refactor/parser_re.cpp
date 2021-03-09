//
// Created by 张承元 on 2021/3/8.
//


#include "parser_re.hpp"

#define REPORT_ERROR_AND_RETURN parser->error = 1;return nullptr;
#define CHECK_ERROR if(parser->error)return nullptr;

void lexicalAnalyse(Parser *parser, FILE *in) {
    parser->token_list = GetTokenList(in);
}

Node *program(Parser *parser) {
    Node *root = GetNode();
    root->type = Program;

    AddChild(root, extDefList(parser));

    // TODO 语法错误传递
    return root;
}

Node *extDefList(Parser *parser) {
    if (CurrentToken(parser->token_list)->type == Eof) {
        return nullptr;
    }
    Node *root = GetNode();
    root->type = ExtDefList;
    AddChild(root, extDef(parser));

    NextToken(parser->token_list); // no need?
    AddChild(root, extDefList(parser));
    return root;
}

Node *extDef(Parser *parser) {


    Token *token = CurrentToken(parser->token_list);

    // 如果 token 不是类型说明符
    if (token->type < INT || token->type < CHAR) {
        REPORT_ERROR_AND_RETURN
    }

    NextToken(parser->token_list);
    token = CurrentToken(parser->token_list);

    if (token->type != IDENT) {
        REPORT_ERROR_AND_RETURN;
    }

    NextToken(parser->token_list);
    token = CurrentToken(parser->token_list);

    if (token->type != LP) {
        return extVarDef(parser);
    } else return funcDef(parser);

}

Node *extVarDef(Parser *parser) {
    Node *root = GetNode();
    root->type = ExtVarDef;

    Node *type_id = GetNode();
    type_id->token = TokenAt(parser->token_list, -2);

    AddChild(root, type_id);

    AddChild(root, declaratorList(parser));

    return root;
}

// <函数定义>：：=<类型说明符> <函数名> (<形式参数序列>) {<复合语句序列>}  (可无)
Node *funcDef(Parser *parser) {

    Node *root = GetNode();
    root->type = FuncDef;

    Node *type_id = GetNode();
    type_id->type = TokenType;
    type_id->token = TokenAt(parser->token_list, -2);
    AddChild(root, type_id);

    Node *func_name = GetNode();
    func_name->type = TokenType;
    func_name->token = TokenAt(parser->token_list, -1);
    AddChild(root, func_name);

    NextToken(parser->token_list);
    AddChild(root, formalParaList(parser));


    Token *token = NextToken(parser->token_list);

    if (token->type == SEMI) {
        return root;
    }
    if (token->type != LCP) {
        DeleteNode(root);
        REPORT_ERROR_AND_RETURN
    }

    Node *curly_braces = GetNode();
    curly_braces->type = CurlyBraces;

    NextToken(parser->token_list);
    AddChild(curly_braces, compoundStatementList(parser));

    AddChild(root, curly_braces);

    return root;
}

Node *declaratorList(Parser *parser) {

    Token *token = CurrentToken(parser->token_list);

    if (token->type != COMMA && token->type != SEMI) {
        REPORT_ERROR_AND_RETURN
    }

    Node *root = GetNode();
    root->type = VarList;
    // 左孩子
    Node *lc = GetNode();
    lc->type = TokenType;
    lc->token = TokenAt(parser->token_list, -1);

    AddChild(root, lc);

    if (token->type == COMMA) {
        token = NextToken(parser->token_list);
        if (token->type != IDENT) {
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

// <形式参数序列>：：=<形式参数> , <形式参数序列>  |  <空>
Node *formalParaList(Parser *parser) {

    Token *token = CurrentToken(parser->token_list);
    if (token->type == RP) {
        return nullptr;
    }

    Node *root = GetNode();
    root->type = FormalParaList;
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
    Token *token = CurrentToken(parser->token_list);
    // 如果不是类型说明符
    if (token->type < INT || token->type > CHAR) {
        REPORT_ERROR_AND_RETURN
    }
    Node *root = GetNode();
    root->type = FormalPara;

    Node *lc = GetNode();
    lc->type = TokenType;
    lc->token = CurrentToken(parser->token_list);
    AddChild(root, lc);

    // need an identity
    token = NextToken(parser->token_list);
    if (token->type != IDENT) {
        DeleteNode(root);
        REPORT_ERROR_AND_RETURN
    }

    Node *rc = GetNode();
    rc->type = TokenType;
    rc->token = CurrentToken(parser->token_list);
    AddChild(root, lc);

    return root;
}

Node *compoundStatementList(Parser *parser) {
    Token *token = CurrentToken(parser->token_list);

    if (token->type == RCP) {
        return nullptr;
    }

    Node *root = GetNode();
    root->type = CompoundSentenceList;

    AddChild(root, compoundStatement(parser));

    // need to forward? yes
    // 退出compoundStatement() token指针会停留在Statement的结束符上

    NextToken(parser->token_list);
    AddChild(root, compoundStatementList(parser));

    return root;
}

// <复合语句> ::= <局部变量定义>  |  <语句>
Node *compoundStatement(Parser *parser) {
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

    Node *root = GetNode();
    root->type = LocalVarDef;

    Node *type_id = GetNode();
    type_id->type = TokenType;
    type_id->token = CurrentToken(parser->token_list);

    AddChild(root, type_id);

    Token *token = NextToken(parser->token_list);
    if (token->type < INT || token->type > CHAR) {
        // 不是类型说明符
        DeleteNode(root);
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
            // token指针此时停留在statement的结束符上

            token = NextToken(parser->token_list);
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
            break;

        case CONTINUE:
            root->type = ContinueStatement;
            break;

        case LCP:
            root->type = BreakStatement;
            NextToken(parser->token_list);
            AddChild(root, compoundStatementList(parser));
            // 从compoundStatementList() 退出时，
            // token指针应停留在 } 上
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
            break;
    }

    // 正常退出时,token指针应停留在 ; 或 } 上
    return root;

}

Node *expression(Parser *parser) {


    return nullptr;
}


int priority(int stdType) {

    int a = stdType;
    if (a == PLUS || a == MINUS) return 0;
    if (a == MULTIPLY || a == DIVIDE) return 1;
    if (a == ASSIGN) return 4;
    if (a == Igt || a == Ige || a == Clt || a == Cle) return 5;
    if (a == EQ || a == UEQ) return 6;
    if (a == AndAnd) return 7;
    if (a == OrOr) return 8;

    return -1;
}
