//
// Created by 张承元 on 2021/3/2.
//

#include "syntax.h"

extern char token_text[20];
extern int then_row; // 当前行数

FILE *fp;// init in main
int token_type; //value from gettoken

// tmp var to save the last res form gettoken
int tmp_type;
char tmp_token[20];

Node *extDefList();

Node *extDef();

Node *extVarDef();

Node *funcDef();

Node *varList();

Node *program() {
    token_type = gettoken(fp);
    Node *root = (Node *) malloc(sizeof(Node));
    root->type = Program;
    root->val.children[0] = extDefList();

    // TODO 语法错误传递

    return root;
}

Node *extDefList() {
    if (token_type == EOF) return nullptr;

    Node *root = (Node *) malloc(sizeof(Node));
    root->type = ExtDefList;
    root->val.children[0] = extDef();
    root->val.children[1] = extDefList();
    return root;
}

Node *extDef() {
    if (token_type < INT || token_type > CHAR) {
        // TODO: error display
        return nullptr;
    }
    tmp_type = token_type;

    token_type = gettoken(fp);
    if (token_type != IDENT) {
        // TODO: error display
        return nullptr;
    }

    strcpy(tmp_token, token_text);

    token_type = gettoken(fp);
    if (token_type != LP) {
        return extVarDef();
    } else return funcDef();

}

Node *funcDef() {
    return nullptr;
}

Node *extVarDef() {
    Node *root = (Node *) malloc(sizeof(Node));
    root->type = ExtVarDef;

    root->val.children[0] = (Node *) malloc(sizeof(Node));
    root->val.children[0]->type = tmp_type;

    root->val.children[1] = varList();

    return root;
}

Node *varList() {
    // 调用前以确保 t
    Node *root = (Node *) malloc(sizeof(Node));
    root->type = VarList;

    // 左孩子
    Node *lc = (Node *) malloc(sizeof(Node));
    lc->type = IDENT;
    strcpy(lc->val.text, tmp_token);

    root->val.children[0] = lc;

    // 开始识别后续变量
    token_type = gettoken(fp);

    if (token_type != COMMA && token_type != SEMI) {
        // TODO: error
        return nullptr;
    }
    if (token_type == COMMA) {
        token_type = gettoken(fp);
        root->val.children[1] = varList(); //递归调用
    }
    if (token_type == SEMI) {
        token_type = gettoken(fp);
        root->val.children[1] = nullptr;
        return root;
    }

}
