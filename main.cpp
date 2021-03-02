#include "syntax.h"

int main() {
    FILE *in = fopen("test.in", "r");
    int w;
    while ((w=gettoken(in)) != EOF) {
        printf("%d\n", w);
        printf("%s\n", token_text);

    }
    return 0;
}