#include <stdio.h>

void fnLogBegin(const char *funcName) {
    printf("Begin function %s\n", funcName);
}

void fnLogEnd(const char *funcName) {
    printf("End function %s\n", funcName);
}
