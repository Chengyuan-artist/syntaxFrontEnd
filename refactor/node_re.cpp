//
// Created by 张承元 on 2021/3/8.
//

#include "node_re.hpp"

void AddChild(Node *root, Node *child) {
    root->children[root->child_num++] = child;
}

Node *GetNode() {
    Node *root = (Node *) malloc(sizeof(Node));
    memset(root, 0, sizeof(Node));
    return root;
}

Node *GetNode(enum NodeType type) {
    Node *node = GetNode();
    node->type = type;
    return node;
}


Node *GetNode(Token *token) {
    Node *node = GetNode();
    node->type = TokenType;
    node->token = token;
    return node;
}

void DeleteNode(Node *root) {

    if (root == nullptr) return;
    for (int i = 0; i < root->child_num; ++i) {
        DeleteNode(root->children[i]);
    }
    free(root);
}


