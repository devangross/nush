#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "tokens.h"

// attribution: skeleton for reading different tokens based on tokenize.c by Nat Tuck

// reads the line's arguments and tokenizes into command names, arguments, and operators
svec*
tokenize(const char* text) {
    svec* xs = make_svec();
    int length = strlen(text);
    int ii = 0;

    while (ii < length) {
        if (isspace(text[ii])) {
            ii++;
            continue;
        }
        if (isop(text[ii])) {
            char* op = read_op(text, ii);
            svec_push_back(xs, op);
            ii += strlen(op);
            free(op);
            continue;
        }
        char* non_op = read_non_op(text, ii);
        svec_push_back(xs, non_op);
        ii += strlen(non_op);
        free(non_op);
    }
    return xs;
}

// determines whether a character is part of an operator
int
isop(char c) {
    switch (c) {
        case '|':
        case '&':
        case ';':
        case '=':
        case '$':
            return 1;
        default:
            return 0;
    }
}

// reads non-operators until space or operator reached
char*
read_non_op(const char* text, int ii) {
    int offset = 0;
    while (!(isop(text[ii + offset]) || isspace(text[ii + offset]))) {
        offset++;
    }
    char* token = malloc(offset + 1);
    memcpy(token, text + ii, offset);
    token[offset] = 0;
    return token;
}

// reads operator arguments
char*
read_op(const char* text, int ii) {
    int offset = 0;
    char goal = text[ii];
    while (text[ii + offset] == goal) {
        offset++;
    }
    char* token = malloc(offset + 1);
    memcpy(token, text + ii, offset);
    token[offset] = 0;
    return token;

}

// reverses the order of the svec
svec*
reverse(svec* xs) {
    int ii = 0;
    int size = xs->size;
    while (ii < (xs->size / 2)) {
        svec_swap(xs, ii, size - ii -1);
        ii++;
    }
    return xs;
}
