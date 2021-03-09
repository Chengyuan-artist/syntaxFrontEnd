//
// Created by 张承元 on 2021/3/8.
//


#include "parser_re.hpp"

#define REPORT_ERROR_AND_RETURN parser->error = 1;return nullptr;
#define CHECK_ERROR if(parser->error)return nullptr;

void lexicalAnalyse(Parser *parser, FILE *in) {

}

Node *program(Parser *parser) {
    Node *root = GetNode();
    root->type = Program;

    AddChild(root, extDefList(parser));

    // TODO 语法错误传递
    return root;
}

Node *extDefList(Parser *parser) {
    if (CurrentToken(parser->token_list) == nullptr) {
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

    AddChild(root, varList(parser));

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
    NextToken(parser->token_list);
    AddChild(root, compoundSentenceList(parser));

    return root;
}

Node *varList(Parser *parser) {

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
        AddChild(root, varList(parser)); // 递归调用
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

Node *compoundSentenceList(Parser *parser) {
    Token *token = CurrentToken(parser->token_list);

    if (token->type == RCP) {
        return nullptr;
    }

    Node *root = GetNode();
    root->type = CompoundSentenceList;

    AddChild(root, compoundSentence(parser));

    // need to forward?
    AddChild(root, compoundSentenceList(parser));

    return root;
}

// <复合语句> ::= <局部变量定义>  |  <语句>
Node *compoundSentence(Parser *parser) {
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

    AddChild(root, varList(parser));

    return root;
}

/* <语句> ::= <表达式>; | return  <表达式>;
           | {<复合语句序列>}
           | if (<表达式>) <语句>
		   | if (<表达式>) <语句> else <语句>
           | while (<表达式>) <语句>
           | for (<表达式>;<表达式>;<表达式>) <语句>
           | continue; | break;
           | <空>;
*/
Node *statement(Parser *parser) {
    CHECK_ERROR
    Node *root;

    Token *token = CurrentToken(parser->token_list);

    if (token->type >= IDENT && token->type <= CHAR_CONST) { // 表达式的必要条件

        root = GetNode();
        root->type = SingleExpStatement;
        AddChild(root, expression(parser));
        if (token->type == SEMI)return root;
        REPORT_ERROR_AND_RETURN
    }

    switch (token->type) {
        case RETURN:
            root = GetNode();
            root->type = ReturnStatement;
            token = NextToken(parser->token_list);
            if (token->type >= IDENT && token->type <= CHAR_CONST) { // 表达式的必要条件
                AddChild(root, expression(parser));
                // 跳出 expression() 时，token已经读取
                if (token->type == SEMI) {
                    return root;
                }
                REPORT_ERROR_AND_RETURN
            }
            // 非表达式
            REPORT_ERROR_AND_RETURN
            break;
        case IF:// 需判断为if或if-else句式
            token = NextToken(parser->token_list);
            if (token->type != LP) {
                REPORT_ERROR_AND_RETURN
            }
            token = NextToken(parser->token_list);
            if (token->type >= IDENT && token->type <= CHAR_CONST) { // 表达式的必要条件
                root = GetNode();
                AddChild(root, expression(parser));

                if (token->type != RP) {
                    REPORT_ERROR_AND_RETURN
                }
                token = NextToken(parser->token_list);
                AddChild(root, statement(parser)); // 递归调用

                token = NextToken(parser->token_list);
                if (token->type == ELSE) {
                    root->type = IfElseStatement;

                    token = NextToken(parser->token_list);
                    AddChild(root, statement(parser));

                    token = NextToken(parser->token_list); //多读一步token
                } else {
                    root->type = IfStatement;
                }
                return root;
            }
            REPORT_ERROR_AND_RETURN
            break;
        case LCP: // {<复合语句序列>}
            NextToken(parser->token_list);
            root = compoundSentenceList(parser);
            return root;
        default:
        REPORT_ERROR_AND_RETURN
    }
}

Node *expression(Parser *parser) {
    return nullptr;
}
