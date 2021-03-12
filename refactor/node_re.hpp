//
// Created by 张承元 on 2021/3/8.
//

#include "lexical_re.hpp"

#ifndef SYNTAXFRONTEND_NODE_RE_HPP
#define SYNTAXFRONTEND_NODE_RE_HPP

enum NodeType{
    NoType, Program, ExtDef,ExtDefList, ExtVarDef, FuncDef, DeclaratorList, FormalParaList, FormalPara,
    CompoundSentenceList, LocalVarDef, WhileStatement,BreakStatement, ContinueStatement,ForStatement,
    SingleExpStatement, ReturnStatement, IfStatement, IfElseStatement,
    CurlyBraces, Parentheses, ArrayDeclarator, ArrayCall, FunctionCall,Expression,
    ArgumentsList,
    TokenType
};

typedef struct Node{
    enum NodeType type;
    Token *token;
    TokenList *annotation_list;
    Node* children[4];
    int child_num;
}Node;

char *ToString(enum NodeType type);
// Public
// Node 的成员函数， 为root添加孩子child
void AddChild(Node *root, Node *child);

// Public
// Node 的成员函数， 相当于 new Node
Node *GetNode();

// 使用NodeType初始化Node
Node *GetNode(enum NodeType type);

// 使用Token初始化Node
Node *GetNode(Token *token);

// Public
// 递归删除结点
void DeleteNode(Node *root);


#endif //SYNTAXFRONTEND_NODE_RE_HPP
