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

    NextToken(parser->token_list);

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



    return root;
}

Node *funcDef(Parser *parser) {
    return nullptr;
}

Node *varList(Parser *parser) {
    return nullptr;
}

Node *formalParaList(Parser *parser) {
    return nullptr;
}

Node *formalPara(Parser *parser) {
    return nullptr;
}

Node *compoundSentenceList(Parser *parser) {
    return nullptr;
}

Node *compoundSentence(Parser *parser) {
    return nullptr;
}

Node *localVarDef(Parser *parser) {
    return nullptr;
}

Node *statement(Parser *parser) {
    return nullptr;
}

Node *expression(Parser *parser) {
    return nullptr;
}
