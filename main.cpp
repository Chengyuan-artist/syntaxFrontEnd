#include "syntax.h"
#include <iostream>
using namespace std;
extern char token_text[20];

int main() {
    FILE *in = fopen("test.in", "r");
    int w;
    while ((w=gettoken(in)) != EOF) {
        printf("%d\n", w);
        printf("%s\n", token_text);
    }

    return 0;
}