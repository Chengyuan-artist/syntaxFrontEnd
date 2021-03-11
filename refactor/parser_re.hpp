//
// Created by 张承元 on 2021/3/7.
//


#include "lexical_re.hpp"
#include "node_re.hpp"


#ifndef SYNTAXFRONTEND_PARSER_HPP
#define SYNTAXFRONTEND_PARSER_RE_HPP

typedef struct Parser{
    FILE *in;
    TokenList *token_list;
    Node *root;

    int error;
    enum ErrorType{

    }error_type;

}Parser;

void visit(Node *root, int layer);
void indent(int layer);

Parser *GetParser(FILE *fp);

void Parsing(Parser *parser);

void LexicalAnalyse(Parser *parser);

Node *program(Parser *parser);

Node *extDefList(Parser *parser);

Node *extDef(Parser *parser);

Node *extVarDef(Parser *parser);

Node *funcDef(Parser *parser);

Node *declaratorList(Parser *parser);

Node *declarator(Parser *parser);

Node *formalParaList(Parser *parser);

Node *formalPara(Parser *parser);

Node *compoundStatementList(Parser *parser);

Node *compoundStatement(Parser *parser);

Node *localVarDef(Parser *parser);

Node *statement(Parser *parser);

Node *expression(Parser *parser);

Node *argumentsList(Parser *parser);

int precede(enum TokenType, enum TokenType);

int priority(enum TokenType);

#endif //SYNTAXFRONTEND_PARSER_HPP

