//
// Created by 张承元 on 2021/3/7.
//


#include "lexical_re.hpp"
#include "node_re.hpp"


#ifndef SYNTAXFRONTEND_PARSER_HPP
#define SYNTAXFRONTEND_PARSER_RE_HPP

typedef struct Parser{
    TokenList *token_list;
    Node *root;

    int error;
    enum ErrorType{

    }error_type;

}Parser;

void lexicalAnalyse(Parser *parser, FILE *in);

Node *program(Parser *parser);

Node *extDefList(Parser *parser);

Node *extDef(Parser *parser);

Node *extVarDef(Parser *parser);

Node *funcDef(Parser *parser);

Node *declaratorList(Parser *parser);

Node *formalParaList(Parser *parser);

Node *formalPara(Parser *parser);

Node *compoundStatementList(Parser *parser);

Node *compoundStatement(Parser *parser);

Node *localVarDef(Parser *parser);

Node *statement(Parser *parser);

Node *expression(Parser *parser);


int priority(int stdType);
#endif //SYNTAXFRONTEND_PARSER_HPP

