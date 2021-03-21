//
// Created by 张承元 on 2021/3/7.
//


#include "lexical.hpp"
#include "node.hpp"


#ifndef SYNTAXFRONTEND_PARSER_HPP
#define SYNTAXFRONTEND_PARSER_RE_HPP

enum ErrorType {
    Token_Error, Need_Specifier,Need_Identifier,
    Need_Semi, Brackets_Not_Match, Type_Not_Match,
    Form_Not_Match, Illegal_Lvalue,Function_Def_Not_Allowed,Else_No_Match,
    File_Error
};

char *ToString(enum ErrorType type);

typedef struct Parser {
    char filename[20];
    TokenList *token_list;
    Node *root;

    int error;
    int error_row;
    enum ErrorType error_type;
    enum NodeType error_pos;

} Parser;

// 记录语法错误
void RecordError(Parser *parser, int error_row,
                 enum NodeType error_pos,
                 enum ErrorType error_type);

void Format(Parser *parser, char *filename);

void visit(Node *root, int layer);

void indent(int layer);

Parser *GetParser(char *filename);

void ReleaseParser(Parser * parser);

void Parsing(Parser *parser);

void PreProcess(Parser *parser);

void LexicalAnalyse(Parser *parser);

void DisplayAllToken(Parser *parser);

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

// 优先级判断
int precede(enum TokenType, enum TokenType);

int priority(enum TokenType);

#endif //SYNTAXFRONTEND_PARSER_HPP

