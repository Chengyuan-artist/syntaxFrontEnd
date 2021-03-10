//
// Created by 张承元 on 2021/3/8.
//

#include "lexical_re.hpp"

#ifndef SYNTAXFRONTEND_NODE_RE_HPP
#define SYNTAXFRONTEND_NODE_RE_HPP

enum NodeType{
    Program, ExtDefList, ExtVarDef, FuncDef, DeclaratorList, FormalParaList, FormalPara,
    CompoundSentenceList, LocalVarDef, WhileStatement,BreakStatement, ContinueStatement,ForStatement,
    SingleExpStatement, ReturnStatement, IfStatement, IfElseStatement,
    CurlyBraces, Parentheses, ArrayDeclarator, Array, Function,Expression,
    ArgumentsList,
    TokenType
};

typedef struct Node{
    enum NodeType type;
    Token *token;
    Node* children[4];
    int child_num;
}Node;


// Public
void AddChild(Node *root, Node *child);

// Public
Node *GetNode();

Node *GetNode(enum NodeType type);

Node *GetNode(Token *token);

// Public
// 递归删除结点
void DeleteNode(Node *root);


#endif //SYNTAXFRONTEND_NODE_RE_HPP
