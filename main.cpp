#include "syntax.hpp"
#include <iostream>
using namespace std;
//extern char token_text[20];
//extern int op_precede_sheet[10][10];
int main() {
    FILE *in = fopen("test.in", "r");

    int a = precede(RP, Begin_End);
    int b =1;

    return 0;
}