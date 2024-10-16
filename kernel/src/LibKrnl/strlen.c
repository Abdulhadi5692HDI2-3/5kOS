#include "libkrnl.h"

int strlen(const char* string) {
    int i = 0;
    while (string[i]) {
        i++;
    }
    return i;
}

int strlen_noconst(char* string) {
    int i = 0;
    while (string[i]) {
        i++;
    }
    return i;
}