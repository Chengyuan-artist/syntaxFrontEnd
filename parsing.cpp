//
// Created by 张承元 on 2021/3/2.
//

#include "syntax.h"
#define CHECK_ERROR if(error)return nullptr;
#define CATCH_ERROR error = 1;

int error = 0;
int error_type;

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

Node *formalParaList();

Node *formalPara();

Node *program() {
    Node *root = (Node *) malloc(sizeof(Node));
    root->type = Program;

    token_type = gettoken(fp);
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
        CATCH_ERROR
        return nullptr;
    }
    tmp_type = token_type;

    token_type = gettoken(fp);
    if (token_type != IDENT) {
        CATCH_ERROR
        return nullptr;
    }

    strcpy(tmp_token, token_text);

    token_type = gettoken(fp);
    if (token_type != LP) {
        return extVarDef();
    } else return funcDef();

}


Node *extVarDef() {
    CHECK_ERROR
    Node *root = (Node *) malloc(sizeof(Node));
    root->type = ExtVarDef;

    root->val.children[0] = (Node *) malloc(sizeof(Node));
    root->val.children[0]->type = tmp_type;

    root->val.children[1] = varList();

    return root;
}

Node *varList() {
    CHECK_ERROR
    // 调用前以确保 tmp_token 存有var的变量名
    if (token_type != COMMA && token_type != SEMI) {
        CATCH_ERROR
        return nullptr;
    }

    Node *root = (Node *) malloc(sizeof(Node));
    root->type = VarList;

    // 左孩子
    Node *lc = (Node *) malloc(sizeof(Node));
    lc->type = IDENT;
    strcpy(lc->val.text, tmp_token);
    root->val.children[0] = lc;


    if (token_type == COMMA) {
        token_type = gettoken(fp);
        if (token_type != IDENT) {
            CATCH_ERROR
            free(lc);free(root);
            return nullptr;
        }
        // 保存变量名
        strcpy(tmp_token, token_text);
        token_type = gettoken(fp);
        root->val.children[1] = varList(); //递归调用
        return root;
    }
    if (token_type == SEMI) {
        token_type = gettoken(fp);
        root->val.children[1] = nullptr;
        return root;
    }
    return nullptr;
}


Node *funcDef() {
    CHECK_ERROR
    Node *root = (Node *) malloc(sizeof(Node));
    root->type = FuncDef;

    Node *type = (Node *) malloc(sizeof(Node));
    type->type = tmp_type;
    root->val.children[0] = type;

    token_type = gettoken(fp);
    root->val.children[1] = formalParaList();

    // TODO: func body part


    return nullptr;
}

Node *formalParaList() {
    CHECK_ERROR
    if (token_type == RP) {
        return nullptr;
    }
    if (token_type < INT || token_type > CHAR) {
        CATCH_ERROR
        return nullptr;
    }

    Node *root = (Node *) malloc(sizeof(Node));
    root->type = FormalParaList;

    root->val.children[0] = formalPara();

    token_type = gettoken(fp);
    if (token_type == COMMA) {
        token_type = gettoken(fp);
        if (token_type < INT || token_type > CHAR) { // 确保 etc. int func(int a,) 不合法
            CATCH_ERROR
            return nullptr;
        }
        root->val.children[1] = formalParaList();
        return root;
    }
    if (token_type == LP){
        root->val.children[1] = nullptr;
        return root;
    }


    return nullptr;
}

Node *formalPara() {
    CHECK_ERROR
    tmp_type = token_type;

    token_type = gettoken(fp);
    if (token_type != IDENT) {
        CATCH_ERROR
        return nullptr;
    }
    Node *root = (Node *) malloc(sizeof(Node));
    root->type = FormalPara;

    Node *lc = (Node *) malloc(sizeof(Node));
    lc->type = tmp_type;
    Node *rc = (Node *) malloc(sizeof(Node));
    rc->type = IDENT;
    strcpy(rc->val.text, token_text);

    root->val.children[0] = lc;
    root->val.children[1] = rc;

    return root;
}
